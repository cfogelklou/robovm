/*
 * Copyright (C) 2012 RoboVM AB
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <robovm.h>
#include "uthash.h"
#include "utlist.h"
#include "MurmurHash3.h"
#include "classinfo.h"

#define LOG_TAG "bc"

#define ALLOC_NATIVE_FRAMES_SIZE 8

typedef struct {
    ClassInfoHeader* classInfoHeader;
    void* start;
    void* end;
} AddressClassLookup;

typedef struct {
    ClassInfoHeader* exHeader;
    jint landingPadId;
} LandingPad;

typedef struct {
    TrycatchContext tc;
    LandingPad** landingPads;
} BcTrycatchContext;

const char* __attribute__ ((weak)) _bcMainClass = NULL;
extern jboolean _bcDynamicJNI;
extern char** _bcStaticLibs;
extern char** _bcBootclasspath;
extern char** _bcClasspath;
extern void* _bcBootClassesHash;
extern void* _bcClassesHash;
static Class* loadBootClass(Env*, const char*, ClassLoader*);
static Class* loadUserClass(Env*, const char*, ClassLoader*);
static void classInitialized(Env*, Class*);
static Interface* loadInterfaces(Env*, Class*);
static Field* loadFields(Env*, Class*);
static Method* loadMethods(Env*, Class*);
static Class* findClassAt(Env*, void*);
static Class* createClass(Env*, ClassInfoHeader*, ClassLoader*);
static jboolean exceptionMatch(Env* env, TrycatchContext*);
static ObjectArray* listBootClasses(Env*, Class*);
static ObjectArray* listUserClasses(Env*, Class*);
static Options options = {0};
static VM* vm = NULL;
static jint addressClassLookupsCount = 0;
static AddressClassLookup* addressClassLookups = NULL;

// ----------------------------------------------------------------
void _TPRINT_CP( const char * const fname, const char * const pc ) {
    fprintf(stderr, "%s = %s\n", fname, pc);
}
#define TPRINT_CP( pc ) _TPRINT_CP( #pc, pc )

// ----------------------------------------------------------------
void _TPRINT_CPP( char * const fname, char ** const pc, const int n ) {
    for (int i = 0; i < n; i++) {
        fprintf(stderr, "%s[%d] = %s\n", fname, i, pc[i]);
    }
}
#define TPRINT_CPP( pc, n ) _TPRINT_CPP( #pc, pc, n )

// ----------------------------------------------------------------
void _TPRINT_INT( const char * const fname, const int n ) {
    fprintf(stderr, "%s = %d\n", fname, n);
}
#define TPRINT_INT( n ) _TPRINT_INT( #n, n )

// ----------------------------------------------------------------
void _TPRINT_LONG( const char * const fname, const long n ) {
    fprintf(stderr, "%s = %ld\n", fname, n);
}
#define TPRINT_LONG( n ) _TPRINT_LONG( #n, n )


static void bc_options_dbg( Options *pOptions, jboolean printOutputVars ) {
    TPRINT_CP( pOptions->mainClass );
    TPRINT_CPP( pOptions->commandLineArgs, pOptions->commandLineArgsCount );
    TPRINT_INT( pOptions->logLevel );
    TPRINT_LONG( pOptions->maxHeapSize );
    TPRINT_LONG( pOptions->initialHeapSize );
    TPRINT_INT( pOptions->enableGCHeapStats );
    TPRINT_INT( pOptions->enableHooks );
    TPRINT_INT( pOptions->waitForResume );
    TPRINT_INT( pOptions->printPID );
    TPRINT_CP( pOptions->pidFile );
    TPRINT_INT( pOptions->printDebugPort );
    TPRINT_CP( pOptions->debugPortFile );
    if (printOutputVars) {
        TPRINT_CP( pOptions->basePath );
        TPRINT_CP( pOptions->executablePath );
        //TPRINT_CPP( pOptions->rawBootclasspath, 0 );
        //TPRINT_CPP( pOptions->rawClasspath, 0 );
    }
}

static void initOptions() {
    options.mainClass = (char*) _bcMainClass;
    options.rawBootclasspath = _bcBootclasspath;
    options.rawClasspath = _bcClasspath;
    options.loadBootClass = loadBootClass;
    options.loadUserClass = loadUserClass;
    options.classInitialized = classInitialized;
    options.loadInterfaces = loadInterfaces;
    options.loadFields = loadFields;
    options.loadMethods = loadMethods;
    options.findClassAt = findClassAt;
    options.exceptionMatch = exceptionMatch;
    options.dynamicJNI = _bcDynamicJNI;
    options.staticLibs = _bcStaticLibs;
    options.listBootClasses = listBootClasses;
    options.listUserClasses = listUserClasses;
}

int bcmain(int argc, char* argv[]) {

    initOptions();
    {
        fprintf(stderr, "Chris Fogelklou's Own Build Number SIX took %d params!!!\n", argc);
        int i;
        for (i = 0; i < argc; i++) {
            fprintf(stderr, "p%d = %s\n", i, argv[i]);
        }
    }

    bc_options_dbg( &options, FALSE );

    if (!rvmInitOptions(argc, argv, &options, FALSE)) {
        fprintf(stderr, "rvmInitOptions(...) failed!\n");
        bc_options_dbg( &options, TRUE );
        return 1;
    }
    fprintf(stderr, "rvmInitOptions(...) succeeded!\n");
    bc_options_dbg( &options, TRUE );
    Env* env = rvmStartup(&options);
    if (!env) {
        fprintf(stderr, "rvmStartup(...) failed!\n");
        return 1;
    }
    vm = env->vm;
    jint result = rvmRun(env) ? 0 : 1;
    rvmShutdown(env, result);

    return result;
}

#if 1
//__attribute__ ((weak)) main;
int __attribute__ ((weak)) main(int argc, char* argv[]) {
    bcmain( argc, argv );
}
#endif

static ClassInfoHeader** getClassInfosBase(void* hash) {
    hash += sizeof(jint); // Skip count
    jint size = ((uint16_t*) hash)[0];
#ifdef _LP64
    ClassInfoHeader** base  = hash + (size << 1) + 4;
#else
    ClassInfoHeader** base  = hash + (size << 1) + 4;
#endif
    return base;
}

static jint getClassInfosCount(void* hash) {
    return ((jint*) hash)[0];
}

static ClassInfoHeader* lookupClassInfo(Env* env, const char* className, void* hash) {
    ClassInfoHeader** base = getClassInfosBase(hash);
    jint h = 0;
    MurmurHash3_x86_32(className, strlen(className) + 1, 0x1ce79e5c, &h);
    hash += sizeof(jint); // Skip count
    jint size = ((uint16_t*) hash)[0];
    h &= size - 1;
    jint start = ((uint16_t*) hash)[h + 1];
    jint end = ((uint16_t*) hash)[h + 1 + 1];
    jint i;
    for (i = start; i < end; i++) {
        ClassInfoHeader* header = base[i];
        if (header && !strcmp(header->className, className)) {
            return header;
        }
    }
    return NULL;
}

static void iterateClassInfos(Env* env, jboolean (*callback)(Env*, ClassInfoHeader*, MethodInfo*, void*), void* hash, void* data) {
    ClassInfoHeader** base = getClassInfosBase(hash);
    jint count = getClassInfosCount(hash);
    jint i = 0;
    for (i = 0; i < count; i++) {
        ClassInfoHeader* header = base[i];
        if ((header->flags & CI_ERROR) == 0) {
            ClassInfo ci;
            void* p = base[i];
            readClassInfo(&p, &ci);
            skipInterfaceNames(&p, &ci);
            skipFieldInfos(&p, &ci);
            jint j;
            for (j = 0; j < ci.methodCount; j++) {
                MethodInfo mi;
                readMethodInfo(&p, &mi);
                if (!callback(env, base[i], &mi, data)) {
                    break;
                }
            }
        }
    }
}

static ObjectArray* listClasses(Env* env, Class* instanceofClazz, ClassLoader* classLoader, void* hash) {
    if (instanceofClazz && (CLASS_IS_ARRAY(instanceofClazz) || CLASS_IS_PRIMITIVE(instanceofClazz))) {
        return NULL;
    }
    ClassInfoHeader** base = getClassInfosBase(hash);
    jint count = getClassInfosCount(hash);
    jint i = 0;
    jint matches = count;
    TypeInfo* instanceofTypeInfo = instanceofClazz ? instanceofClazz->typeInfo : NULL;
    if (instanceofTypeInfo) {
        matches = 0;
        for (i = 0; i < count; i++) {
            ClassInfoHeader* header = base[i];
            if ((header->flags & CI_ERROR) == 0) {
                if ((!CLASS_IS_INTERFACE(instanceofClazz) 
                    && rvmIsClassTypeInfoAssignable(env, header->typeInfo, instanceofTypeInfo))
                    || (CLASS_IS_INTERFACE(instanceofClazz) 
                    && rvmIsInterfaceTypeInfoAssignable(env, header->typeInfo, instanceofTypeInfo))) {

                    matches++;
                }
            }
        }
    }

    if (matches == 0) return NULL;
    ObjectArray* result = rvmNewObjectArray(env, matches, java_lang_Class, NULL, NULL);
    if (!result) return NULL;

    jint j = 0;
    for (i = 0; i < count; i++) {
        ClassInfoHeader* header = base[i];
        if ((header->flags & CI_ERROR) == 0) {
            if (!instanceofTypeInfo || ((!CLASS_IS_INTERFACE(instanceofClazz) 
                && rvmIsClassTypeInfoAssignable(env, header->typeInfo, instanceofTypeInfo))
                || (CLASS_IS_INTERFACE(instanceofClazz) 
                && rvmIsInterfaceTypeInfoAssignable(env, header->typeInfo, instanceofTypeInfo)))) {

                result->values[j++] = (Object*) (header->clazz ? header->clazz : createClass(env, header, classLoader));
                if (rvmExceptionOccurred(env)) return NULL;
            }
        }
    }
    return result;
}
static ObjectArray* listBootClasses(Env* env, Class* instanceofClazz) {
    return listClasses(env, instanceofClazz, NULL, _bcBootClassesHash);
}
static ObjectArray* listUserClasses(Env* env, Class* instanceofClazz) {
    return listClasses(env, instanceofClazz, systemClassLoader, _bcClassesHash);
}

static Class* loadClass(Env* env, const char* className, ClassLoader* classLoader, void* hash) {
    ClassInfoHeader* header = lookupClassInfo(env, className, hash);
    if (!header) return NULL;
    if (header->flags & CI_ERROR) {
        ClassInfoError* error = (ClassInfoError*) header;
        switch (error->errorType) {
        case CI_ERROR_TYPE_NO_CLASS_DEF_FOUND:
            rvmThrowNoClassDefFoundError(env, error->errorMessage);
            break;
        case CI_ERROR_TYPE_ILLEGAL_ACCESS:
            rvmThrowIllegalAccessError(env, error->errorMessage);
            break;
        case CI_ERROR_TYPE_INCOMPATIBLE_CLASS_CHANGE:
            rvmThrowIncompatibleClassChangeError(env, error->errorMessage);
            break;
        }
        return NULL;
    }
    if (header->clazz) return header->clazz;
    return createClass(env, header, classLoader);
}

static Class* loadBootClass(Env* env, const char* className, ClassLoader* classLoader) {
    return loadClass(env, className, classLoader, _bcBootClassesHash);
}

static Class* loadUserClass(Env* env, const char* className, ClassLoader* classLoader) {
    return loadClass(env, className, classLoader, _bcClassesHash);
}

static void wrapClassNotFoundException(Env* env, const char* className) {
    Object* exception = rvmExceptionOccurred(env);
    if (exception && exception->clazz == java_lang_ClassNotFoundException) {
        // If ClassNotFoundException is thrown we have to wrap it in a NoClassDefFoundError
        exception = rvmExceptionClear(env);
        Method* constructor = rvmGetInstanceMethod(env, java_lang_NoClassDefFoundError, "<init>", "(Ljava/lang/String;)V");
        if (!constructor) return;
        Object* message = rvmNewStringUTF(env, className, -1);
        if (!message) return;
        Object* wrappedException = rvmNewObject(env, java_lang_NoClassDefFoundError, constructor, message);
        if (!wrappedException) return;
        Class* java_lang_StackTraceElement = rvmFindClassUsingLoader(env, "java/lang/StackTraceElement", NULL);
        if (!java_lang_StackTraceElement) return;
        ObjectArray* stackTrace = rvmNewObjectArray(env, 0, java_lang_StackTraceElement, NULL, NULL);
        if (!stackTrace) return;
        Method* setStackTrace = rvmGetInstanceMethod(env, java_lang_Throwable, "setStackTrace", "([Ljava/lang/StackTraceElement;)V");
        if (!setStackTrace) return;
        rvmCallVoidInstanceMethod(env, wrappedException, setStackTrace, stackTrace);
        if (rvmExceptionCheck(env)) return;
        Method* initCause = rvmGetInstanceMethod(env, java_lang_NoClassDefFoundError, "initCause", "(Ljava/lang/Throwable;)Ljava/lang/Throwable;");
        if (!initCause) return;
        rvmCallObjectInstanceMethod(env, wrappedException, initCause, exception);
        if (!rvmExceptionCheck(env)) rvmThrow(env, wrappedException);
    }
}

static Class* createClass(Env* env, ClassInfoHeader* header, ClassLoader* classLoader) {
    ClassInfo ci;
    void* p = header;
    readClassInfo(&p, &ci);

    Class* superclass = NULL;
    if (ci.superclassName) {
        superclass = rvmFindClassUsingLoader(env, ci.superclassName, classLoader);
        if (!superclass) return NULL;
    }

    rvmObtainClassLock(env);

    Class* clazz = rvmAllocateClass(env, header->className, superclass, classLoader, ci.access, header->typeInfo, header->vitable, header->itables,
            header->classDataSize, header->instanceDataSize, header->instanceDataOffset, header->classRefCount, 
            header->instanceRefCount, ci.attributes, header->initializer);

    if (clazz) {
        if (!rvmRegisterClass(env, clazz)) {
            rvmReleaseClassLock(env);
            return NULL;
        }
        header->clazz = clazz;
        rvmHookClassLoaded(env, clazz, (void*)header);
    }

    rvmReleaseClassLock(env);
    
    return clazz;
}

static void classInitialized(Env* env, Class* clazz) {
    ClassInfoHeader* header = lookupClassInfo(env, clazz->name, 
        !clazz->classLoader || !clazz->classLoader->parent ? _bcBootClassesHash : _bcClassesHash);
    if (!header) return;
    rvmAtomicStoreInt(&header->flags, header->flags | CI_INITIALIZED);
}

static Interface* loadInterfaces(Env* env, Class* clazz) {
    ClassInfoHeader* header = lookupClassInfo(env, clazz->name, 
        !clazz->classLoader || !clazz->classLoader->parent ? _bcBootClassesHash : _bcClassesHash);
    if (!header) return NULL;

    ClassInfo ci;
    jint i;
    void* p = header;
    readClassInfo(&p, &ci);

    Interface* first = NULL;
    for (i = 0; i < ci.interfaceCount; i++) {
        const char* interfaceName = readInterfaceName(&p);
        Class* interfaceClass = rvmFindClassUsingLoader(env, interfaceName, clazz->classLoader);
        if (!interfaceClass) goto error;
        Interface* interf = rvmAllocateInterface(env, interfaceClass);
        if (!interf) goto error;
        LL_APPEND(first, interf); // Interfaces has to be in the correct order so we need to use the slower LL_APPEND
    }
    return first;
error:
    while (first) {
        Interface* next = first->next;
        rvmFreeMemoryUncollectable(env, first);
        first = next;
    }
    return NULL;
}

static Field* loadFields(Env* env, Class* clazz) {
    ClassInfoHeader* header = lookupClassInfo(env, clazz->name, 
        !clazz->classLoader || !clazz->classLoader->parent ? _bcBootClassesHash : _bcClassesHash);
    if (!header) return NULL;

    ClassInfo ci;
    jint i;
    void* p = header;
    readClassInfo(&p, &ci);

    skipInterfaceNames(&p, &ci);

    Field* first = NULL;
    for (i = 0; i < ci.fieldCount; i++) {
        FieldInfo fi;
        readFieldInfo(&p, &fi);
        Field* f = rvmAllocateField(env, clazz, fi.name, fi.desc, fi.access, fi.offset, fi.attributes);
        if (!f) goto error;
        LL_PREPEND(first, f);
    }
    return first;
error:
    while (first) {
        Field* next = first->next;
        rvmFreeMemoryUncollectable(env, first);
        first = next;
    }
    return NULL;
}

static Method* loadMethods(Env* env, Class* clazz) {
    ClassInfoHeader* header = lookupClassInfo(env, clazz->name, 
        !clazz->classLoader || !clazz->classLoader->parent ? _bcBootClassesHash : _bcClassesHash);
    if (!header) return NULL;

    ClassInfo ci;
    jint i;
    void* p = header;
    readClassInfo(&p, &ci);
    
    skipInterfaceNames(&p, &ci);
    skipFieldInfos(&p, &ci);

    Method* first = NULL;
    for (i = 0; i < ci.methodCount; i++) {
        MethodInfo mi;
        readMethodInfo(&p, &mi);
        Method* m = NULL;
        if (mi.targetFnPtr) {
            m = (Method*) rvmAllocateBridgeMethod(env, clazz, mi.name, mi.desc, mi.vtableIndex, mi.access, mi.size, mi.impl, mi.synchronizedImpl, mi.targetFnPtr, mi.attributes);
        } else if (mi.callbackImpl) {
            m = (Method*) rvmAllocateCallbackMethod(env, clazz, mi.name, mi.desc, mi.vtableIndex, mi.access, mi.size, mi.impl, mi.synchronizedImpl, mi.linetable, mi.callbackImpl, mi.attributes);
        } else {
            m = rvmAllocateMethod(env, clazz, mi.name, mi.desc, mi.vtableIndex, mi.access, mi.size, mi.impl, mi.synchronizedImpl, mi.linetable, mi.attributes);
        }
        if (!m) goto error;
        LL_PREPEND(first, m);
    }
    return first;
error:
    while (first) {
        Method* next = first->next;
        rvmFreeMemoryUncollectable(env, first);
        first = next;
    }
    return NULL;
}

static jboolean countClassesWithConcreteMethodsCallback(Env* env, ClassInfoHeader* header, MethodInfo* mi, void* d) {
    if (mi->impl) {
        jint* count = (jint*) d;
        *count = *count + 1;
        return FALSE;
    }
    return TRUE;
}

static jboolean initAddressClassLookupsCallback(Env* env, ClassInfoHeader* header, MethodInfo* mi, void* d) {
    if (mi->impl) {
        AddressClassLookup** lookupPtr = (AddressClassLookup**) d;
        AddressClassLookup* lookup = *lookupPtr;
        if (lookup->classInfoHeader != header) {
            if (lookup->classInfoHeader) {
                *lookupPtr += 1;
                lookup = *lookupPtr;
            }
            lookup->classInfoHeader = header;
            lookup->start = mi->impl;
            lookup->end = mi->impl + mi->size;
        } else if (lookup->start > mi->impl) {
            lookup->start = mi->impl;
        } else if (lookup->end <= mi->impl) {
            lookup->end = mi->impl + mi->size;
        }
    }
    return TRUE;
}

static int addressClassLookupCompareBSearch(const void* _a, const void* _b) {
    AddressClassLookup* needle = (AddressClassLookup*) _a;
    AddressClassLookup* el = (AddressClassLookup*) _b;
    void* pc = needle->start;
    return (pc >= el->start && pc < el->end) ? 0 : ((pc < el->start) ? -1 : 1);
}

static int addressClassLookupCompareQSort(const void* _a, const void* _b) {
    AddressClassLookup* a = (AddressClassLookup*) _a;
    AddressClassLookup* b = (AddressClassLookup*) _b;
    return (a->end <= b->start) ? -1 : 1;
}

static AddressClassLookup* getAddressClassLookups(Env* env) {
    if (!addressClassLookups) {
        jint count = 0;
        iterateClassInfos(env, countClassesWithConcreteMethodsCallback, _bcBootClassesHash, &count);
        iterateClassInfos(env, countClassesWithConcreteMethodsCallback, _bcClassesHash, &count);
        AddressClassLookup* lookups = rvmAllocateMemoryAtomicUncollectable(env, sizeof(AddressClassLookup) * count);
        if (!lookups) return NULL;
        AddressClassLookup* _lookups = lookups;
        iterateClassInfos(env, initAddressClassLookupsCallback, _bcBootClassesHash, &_lookups);
        iterateClassInfos(env, initAddressClassLookupsCallback, _bcClassesHash, &_lookups);
        qsort(lookups, count, sizeof(AddressClassLookup), addressClassLookupCompareQSort);
        addressClassLookupsCount = count;
        addressClassLookups = lookups;
    }
    return addressClassLookups;
}

Class* findClassAt(Env* env, void* pc) {
    AddressClassLookup* lookups = getAddressClassLookups(env);
    if (!lookups) return NULL;
    AddressClassLookup needle = {NULL, pc, pc};
    AddressClassLookup* result = bsearch(&needle, lookups, addressClassLookupsCount, sizeof(AddressClassLookup), addressClassLookupCompareBSearch);
    if (!result) return NULL;
    ClassInfoHeader* header = result->classInfoHeader;
    Class* clazz = header->clazz;
    if (!clazz) {
        ClassLoader* loader = NULL;
        if (lookupClassInfo(env, header->className, _bcClassesHash) == header) {
            loader = systemClassLoader;
        }
        clazz = rvmFindClassUsingLoader(env, header->className, loader);
    }
    return clazz;
}

jboolean exceptionMatch(Env* env, TrycatchContext* _tc) {
    BcTrycatchContext* tc = (BcTrycatchContext*) _tc;
    LandingPad* lps = tc->landingPads[tc->tc.sel - 1];
    Object* throwable = rvmExceptionOccurred(env);
    jint i;
    for (i = 0; lps[i].landingPadId > 0; i++) {
        ClassInfoHeader* header = lps[i].exHeader;
        if (!header) {
            // NULL means java.lang.Throwable which always matches
            tc->tc.sel = lps[i].landingPadId;
            return TRUE;
        }
        if (!header->clazz) {
            // Exception class not yet loaded so it cannot match.
            continue;
        }
        Class* clazz = header->clazz;
        Class* c = throwable->clazz;
        while (c && c != clazz) {
            c = c->superclass;
        }
        if (c == clazz) {
            tc->tc.sel = lps[i].landingPadId;
            return TRUE;
        }
    }
    return FALSE;
}

#define ENTER rvmPushGatewayFrame(env)
#define LEAVEV \
    rvmPopGatewayFrame(env); \
    if (rvmExceptionCheck(env)) rvmRaiseException(env, rvmExceptionOccurred(env))
#define LEAVE(result) \
    rvmPopGatewayFrame(env); \
    if (rvmExceptionCheck(env)) rvmRaiseException(env, rvmExceptionOccurred(env)); \
    return result

static Class* ldcClass(Env* env, ClassInfoHeader* header) {
    Class* clazz = header->clazz;
    if (!clazz) {
        ClassLoader* loader = NULL;
        if (lookupClassInfo(env, header->className, _bcClassesHash) == header) {
            loader = systemClassLoader;
        }
        clazz = rvmFindClassUsingLoader(env, header->className, loader);
        if (!clazz) wrapClassNotFoundException(env, header->className);
    }
    return clazz;
}
static void initializeClass(Env* env, ClassInfoHeader* header) {
    Class* clazz = ldcClass(env, header);
    if (clazz) rvmInitialize(env, clazz);
}
void _bcInitializeClass(Env* env, ClassInfoHeader* header) {
    ENTER;
    initializeClass(env, header);
    LEAVEV;
}

static void* lookupVirtualMethod(Env* env, Object* thiz, char* name, char* desc) {
    Method* method = rvmGetMethod(env, thiz->clazz, name, desc);
    if (!method) return NULL;
    if (METHOD_IS_ABSTRACT(method)) {
        rvmThrowAbstractMethodError(env, ""); // TODO: Message
        return NULL;
    }
    return method->synchronizedImpl ? method->synchronizedImpl : method->impl;
}
void* _bcLookupVirtualMethod(Env* env, Object* thiz, char* name, char* desc) {
    ENTER;
    void* result = lookupVirtualMethod(env, thiz, name, desc);
    LEAVE(result);
}

void* lookupInterfaceMethod(Env* env, ClassInfoHeader* header, Object* thiz, char* name, char* desc) {
    initializeClass(env, header);
    if (rvmExceptionCheck(env)) return NULL;
    Class* ownerInterface = header->clazz;
    if (!rvmIsInstanceOf(env, thiz, ownerInterface)) {
        char message[256];
        snprintf(message, 256, "Class %s does not implement the requested interface %s", 
            rvmToBinaryClassName(env, thiz->clazz->name), rvmToBinaryClassName(env, ownerInterface->name));
        rvmThrowIncompatibleClassChangeError(env, message);
        return NULL;
    }
    Method* method = rvmGetInstanceMethod(env, thiz->clazz, name, desc);
    Object* throwable = rvmExceptionClear(env);
    if (!method && throwable->clazz != java_lang_NoSuchMethodError) { 
        rvmThrow(env, throwable);
        return NULL;
    }
    if (!method || METHOD_IS_ABSTRACT(method)) {
        rvmThrowAbstractMethodError(env, ""); // TODO: Message
        return NULL;
    }
    if (!METHOD_IS_PUBLIC(method)) {
        rvmThrowIllegalAccessError(env, ""); // TODO: Message
        return NULL;
    }
    return method->synchronizedImpl ? method->synchronizedImpl : method->impl;
}
void* _bcLookupInterfaceMethod(Env* env, ClassInfoHeader* header, Object* thiz, char* name, char* desc) {
    ENTER;
    void* result = lookupInterfaceMethod(env, header, thiz, name, desc);
    LEAVE(result);
}

void* _bcLookupInterfaceMethodImpl(Env* env, ClassInfoHeader* header, Object* thiz, uint32_t index) {
    TypeInfo* typeInfo = header->typeInfo;
    ITables* itables = thiz->clazz->itables;
    ITable* itable = itables->cache;
    if (itable->typeInfo == typeInfo) {
        return itable->table.table[index];
    }
    uint32_t i;
    for (i = 0; i < itables->count; i++) {
        itable = itables->table[i];
        if (itable->typeInfo == typeInfo) {
            itables->cache = itable;
            return itable->table.table[index];
        }
    }

    ENTER;
    initializeClass(env, header);
    Class* ownerInterface = header->clazz;
    char message[256];
    snprintf(message, 256, "Class %s does not implement the requested interface %s", 
        rvmToBinaryClassName(env, thiz->clazz->name), rvmToBinaryClassName(env, ownerInterface->name));
    rvmThrowIncompatibleClassChangeError(env, message);
    LEAVE(NULL);
}

void _bcAbstractMethodCalled(Env* env, Object* thiz) {
    ENTER;
    char msg[256];
    char* name = env->reserved0;
    char* desc = env->reserved1;
    snprintf(msg, sizeof(msg), "%s.%s%s", thiz->clazz->name, name, desc);
    char* s = msg;
    while (*s != '\0') {
        if (*s == '/') *s = '.';
        s++;
    }
    rvmThrowAbstractMethodError(env, msg);
    LEAVEV;
}

void _bcNonPublicMethodCalled(Env* env, Object* thiz) {
    ENTER;
    char msg[256];
    char* name = env->reserved0;
    char* desc = env->reserved1;
    snprintf(msg, sizeof(msg), "%s.%s%s not public", thiz->clazz->name, name, desc);
    char* s = msg;
    while (*s != '\0') {
        if (*s == '/') *s = '.';
        s++;
    }
    rvmThrowIllegalAccessError(env, msg);
    LEAVEV;
}

void _bcMoveMemory16(void* dest, const void* src, jlong n) {
    rvmMoveMemory16(dest, src, n);
}

void _bcMoveMemory32(void* dest, const void* src, jlong n) {
    rvmMoveMemory32(dest, src, n);
}

void _bcTrycatchLeave(Env* env) {
    rvmTrycatchLeave(env);
}

void _bcThrow(Env* env, Object* throwable) {
    rvmRaiseException(env, throwable);
}

void _bcThrowIfExceptionOccurred(Env* env) {
    Object* throwable = rvmExceptionOccurred(env);
    if (throwable) rvmRaiseException(env, throwable);
}

Object* _bcExceptionClear(Env* env) {
    return rvmExceptionClear(env);
}

void _bcThrowNullPointerException(Env* env) {
    ENTER;
    rvmThrowNullPointerException(env);
    LEAVEV;
}

void _bcThrowArrayIndexOutOfBoundsException(Env* env, jint length, jint index) {
    ENTER;
    rvmThrowArrayIndexOutOfBoundsException(env, length, index);
    LEAVEV;
}

void _bcThrowArithmeticException(Env* env) {
    ENTER;
    rvmThrowArithmeticException(env);
    LEAVEV;
}

void _bcThrowUnsatisfiedLinkError(Env* env, char* msg) {
    ENTER;
    rvmThrowUnsatisfiedLinkError(env, msg);
    LEAVEV;
}

void _bcThrowNoClassDefFoundError(Env* env, char* msg) {
    ENTER;
    rvmThrowNoClassDefFoundError(env, msg);
    LEAVEV;
}

void _bcThrowNoSuchFieldError(Env* env, char* msg) {
    ENTER;
    rvmThrowNoSuchFieldError(env, msg);
    LEAVEV;
}

void _bcThrowNoSuchMethodError(Env* env, char* msg) {
    ENTER;
    rvmThrowNoSuchMethodError(env, msg);
    LEAVEV;
}

void _bcThrowIllegalAccessError(Env* env, char* msg) {
    ENTER;
    rvmThrowIllegalAccessError(env, msg);
    LEAVEV;
}

void _bcThrowInstantiationError(Env* env, char* msg) {
    ENTER;
    rvmThrowInstantiationError(env, msg);
    LEAVEV;
}

void _bcThrowAbstractMethodError(Env* env, char* msg) {
    ENTER;
    rvmThrowAbstractMethodError(env, msg);
    LEAVEV;
}

void _bcThrowIncompatibleClassChangeError(Env* env, char* msg) {
    ENTER;
    rvmThrowIncompatibleClassChangeError(env, msg);
    LEAVEV;
}

void _bcThrowClassCastException(Env* env, ClassInfoHeader* header, Object* o) {
    ENTER;
    Class* clazz = ldcClass(env, header);
    rvmThrowClassCastException(env, clazz, o->clazz);
    LEAVEV;
}

void _bcThrowClassCastExceptionArray(Env* env, Class* arrayClass, Object* o) {
    ENTER;
    rvmThrowClassCastException(env, arrayClass, o->clazz);
    LEAVEV;
}

Object* _bcAllocate(Env* env, ClassInfoHeader* header) {
    ENTER;
    Object* obj = rvmAllocateObject(env, header->clazz);
    LEAVE(obj);
}

void _bcRegisterFinalizer(Env* env, Object* obj) {
    ENTER;
    rvmRegisterFinalizer(env, obj);
    LEAVEV;
}

BooleanArray* _bcNewBooleanArray(Env* env, jint length) {
    ENTER;
    BooleanArray* array = rvmNewBooleanArray(env, length);
    LEAVE(array);
}

ByteArray* _bcNewByteArray(Env* env, jint length) {
    ENTER;
    ByteArray* array = rvmNewByteArray(env, length);
    LEAVE(array);
}

CharArray* _bcNewCharArray(Env* env, jint length) {
    ENTER;
    CharArray* array = rvmNewCharArray(env, length);
    LEAVE(array);
}

ShortArray* _bcNewShortArray(Env* env, jint length) {
    ENTER;
    ShortArray* array = rvmNewShortArray(env, length);
    LEAVE(array);
}

IntArray* _bcNewIntArray(Env* env, jint length) {
    ENTER;
    IntArray* array = rvmNewIntArray(env, length);
    LEAVE(array);
}

LongArray* _bcNewLongArray(Env* env, jint length) {
    ENTER;
    LongArray* array = rvmNewLongArray(env, length);
    LEAVE(array);
}

FloatArray* _bcNewFloatArray(Env* env, jint length) {
    ENTER;
    FloatArray* array = rvmNewFloatArray(env, length);
    LEAVE(array);
}

DoubleArray* _bcNewDoubleArray(Env* env, jint length) {
    ENTER;
    DoubleArray* array = rvmNewDoubleArray(env, length);
    LEAVE(array);
}

ObjectArray* _bcNewObjectArray(Env* env, jint length, Class* arrayClass) {
    ENTER;
    ObjectArray* array = rvmNewObjectArray(env, length, NULL, arrayClass, NULL);
    LEAVE(array);
}

Array* _bcNewMultiArray(Env* env, jint dims, jint* lengths, Class* arrayClass) {
    ENTER;
    Array* array = rvmNewMultiArray(env, dims, lengths, arrayClass);
    LEAVE(array);
}

void _bcSetObjectArrayElement(Env* env, ObjectArray* array, jint index, Object* value) {
    if (!value) {
        array->values[index] = value;
        return;        
    }
    ENTER;
    Class* componentType = array->object.clazz->componentType;
    jboolean assignable = rvmIsAssignableFrom(env, value->clazz, componentType);
    if (!rvmExceptionCheck(env) && !assignable) {
        rvmThrowArrayStoreException(env, value->clazz, array->object.clazz);
    }
    if (!rvmExceptionCheck(env)) array->values[index] = value;
    LEAVEV;
}


Object* _bcLdcString(Env* env, Object** ptr, char* s, jint length) {
    Object* o = *ptr;
    if (o) return o;
    ENTER;
    o = rvmNewInternedStringUTF(env, s, length);
    if (!rvmExceptionCheck(env)) {
        *ptr = o;
        rvmRegisterDisappearingLink(env, (void**) ptr, o);
    }
    LEAVE(o);
}

Object* _bcLdcArrayBootClass(Env* env, Class** arrayClassPtr, char* name) {
    Class* arrayClass = *arrayClassPtr;
    if (arrayClass) return (Object*) arrayClass;
    ENTER;    
    arrayClass = rvmFindClassUsingLoader(env, name, NULL);
    wrapClassNotFoundException(env, name);
    if (!rvmExceptionCheck(env)) {
        *arrayClassPtr = arrayClass;
    }
    LEAVE((Object*) arrayClass);
}

Object* _bcLdcArrayClass(Env* env, Class** arrayClassPtr, char* name) {
    Class* arrayClass = *arrayClassPtr;
    if (arrayClass) return (Object*) arrayClass;
    ENTER;
    arrayClass = rvmFindClassUsingLoader(env, name, systemClassLoader);
    wrapClassNotFoundException(env, name);
    if (!rvmExceptionCheck(env)) {
        *arrayClassPtr = arrayClass;
    }
    LEAVE((Object*) arrayClass);
}

Object* _bcLdcClass(Env* env, ClassInfoHeader* header) {
    Class* clazz = header->clazz;
    if (clazz) return (Object*) clazz;
    ENTER;
    clazz = ldcClass(env, header);
    LEAVE((Object*) clazz);
}


void _bcMonitorEnter(Env* env, Object* obj) {
    ENTER;
    rvmLockObject(env, obj);
    LEAVEV;
}

void _bcMonitorExit(Env* env, Object* obj) {
    ENTER;
    rvmUnlockObject(env, obj);
    LEAVEV;
}

Object* _bcCheckcast(Env* env, ClassInfoHeader* header, Object* o) {
    if (!o) return o;
    ENTER;
    Class* clazz = ldcClass(env, header);
    if (clazz) {
        jboolean b = rvmIsAssignableFrom(env, o->clazz, clazz);
        if (!rvmExceptionCheck(env) && !b) {
            rvmThrowClassCastException(env, clazz, o->clazz);
        }
    }
    LEAVE(o);
}

Object* _bcCheckcastArray(Env* env, Class* arrayClass, Object* o) {
    if (!o) return o;
    ENTER;
    jboolean b = rvmIsAssignableFrom(env, o->clazz, arrayClass);
    if (!rvmExceptionCheck(env) && !b) {
        rvmThrowClassCastException(env, arrayClass, o->clazz);
    }
    LEAVE(o);
}

jint _bcInstanceof(Env* env, ClassInfoHeader* header, Object* o) {
    if (!o) return (jint) FALSE;
    ENTER;
    Class* clazz = ldcClass(env, header);
    jboolean b = FALSE;
    if (clazz) {
        b = rvmIsInstanceOf(env, o, clazz);
    }
    LEAVE((jint) b);
}

jint _bcInstanceofArray(Env* env, Class* arrayClass, Object* o) {
    if (!o) return (jint) FALSE;
    ENTER;
    jboolean b = rvmIsInstanceOf(env, o, arrayClass);
    LEAVE((jint) b);
}

void _bcPushNativeFrame(Env* env, GatewayFrame* gwFrame, void* frameAddress) {
    rvmPushGatewayFrame0(env, gwFrame, frameAddress, NULL);
}

void _bcPopNativeFrame(Env* env) {
    rvmPopGatewayFrame(env);
}

void _bcPushCallbackFrame(Env* env, GatewayFrame* gwFrame, void* frameAddress) {
    rvmPushGatewayFrame0(env, gwFrame, frameAddress, NULL);
}

void _bcPopCallbackFrame(Env* env) {
    rvmPopGatewayFrame(env);
}

void* _bcResolveNative(Env* env, Class* clazz, char* name, char* desc, char* shortMangledName, char* longMangledName, void** ptr) {
    if (*ptr != NULL) return *ptr;
    ENTER;
    TRACEF("_bcResolveNative: owner=%s, name=%s, desc=%s, shortMangledName=%s, longMangledName=%s", 
        clazz->name, name, desc, shortMangledName, longMangledName);
    NativeMethod* method = (NativeMethod*) rvmGetMethod(env, clazz, name, desc);
    void* impl = NULL;
    if (method) {
        impl = rvmResolveNativeMethodImpl(env, method, shortMangledName, longMangledName, clazz->classLoader, ptr);
    }
    LEAVE(impl);
}

Env* _bcAttachThreadFromCallback(void) {
    Env* env = rvmGetEnv();
    if (!env) {
        // This thread has never been attached or it has been
        // attached, then detached in the TLS destructor. In the
        // latter case, we are getting called back by native code
        // e.g. an auto-release pool, that is triggered after
        // the TLS destructor.
        if (rvmAttachCurrentThreadAsDaemon(vm, &env, NULL, NULL) != JNI_OK) {
            rvmAbort("Failed to attach thread in callback");
        }
    }
    return env;
}

void _bcDetachThreadFromCallback(Env* env) {
    if(rvmHasThreadBeenDetached()) {
        // this can only ever be called after
        // the TLS destructor for this thread
        // has been called. It happens when
        // an auto-release pool calls back
        // into our Java code. In that case
        // bcAttachThreadFromCallback must
        // reattach the thread first.
        rvmDetachCurrentThread(vm, TRUE, TRUE);
    }
}

void* _bcCopyStruct(Env* env, void* src, jint size) {
    ENTER;
    void* result = rvmCopyMemory(env, src, size);
    LEAVE(result);
}

void _bcHookInstrumented(DebugEnv* debugEnv, jint lineNumber, jint lineNumberOffset, jbyte* bptable, void* pc) {
    Env* env = (Env*) debugEnv;
    // Temporarily clear any exception that has been thrown. Could be the case if we are called from a catch block.
    Object* throwable = rvmExceptionClear(env);
    // We cannot use ENTER/LEAVEV as it will raise any exception thrown in here. We just want the code to 
    // proceed normally after we return.
    rvmPushGatewayFrame(env);
    rvmHookInstrumented(debugEnv, lineNumber, lineNumberOffset, bptable, pc);
    rvmPopGatewayFrame(env);
    // Restore the exception if one had been thrown when this function was called.
    env->throwable = throwable;
}


/*
  JNI_GetDefaultJavaVMInitArgs()

  Returns a default configuration for the Java VM. Before calling this function,
  native code must set the vm_args->version field to the JNI version it expects
  the VM to support. After this function returns, vm_args->version will be set
  to the actual JNI version the VM supports.

  LINKAGE:
  Exported from the native library that implements the Java virtual machine.

  PARAMETERS:
  vm_args: a pointer to a JavaVMInitArgs structure in to which the default
  arguments are filled.

  RETURNS:
  Returns JNI_OK if the requested version is supported; returns a JNI error code
  (a negative number) if the requested version is not supported.
 */
jint JNI_GetDefaultJavaVMInitArgs(void* vm_args) {
    return JNI_OK;
}

/*
  Loads and initializes a Java VM. The current thread becomes the main thread.
  Sets the env argument to the JNI interface pointer of the main thread.

  As of JDK/JRE 1.2 , creation of multiple VMs in a single process is not supported.
  The second argument to JNI_CreateJavaVM is always a pointer to JNIEnv *, while
  the third argument is a pointer to a JavaVMInitArgs structure which uses option
  strings to encode arbitrary VM start up options:

  typedef struct JavaVMInitArgs {
      jint version;

      jint nOptions;
      JavaVMOption *options;
      jboolean ignoreUnrecognized;
  } JavaVMInitArgs;

  The version field must be set to at least JNI_VERSION_1_2. The options field is
  an array of the following type:

  typedef struct JavaVMOption {
      char *optionString;  // the option as a string in the default platform encoding
      void *extraInfo;
  } JavaVMOption;

  The size of the array is denoted by the nOptions field in JavaVMInitArgs. If
  ignoreUnrecognized is JNI_TRUE, JNI_CreateJavaVM ignore all unrecognized option
  strings that begin with "-X" or "_". If ignoreUnrecognized is JNI_FALSE,
  JNI_CreateJavaVM returns JNI_ERR as soon as it encounters any unrecognized
  option strings. All Java VMs must recognize the following set of standard options:

  optionString    meaning
    -D<name>=<value>    Set a system property
    -verbose[:class|gc|jni] Enable verbose output. The options can be followed
    by a comma-separated list of names indicating what kind of messages will
    be printed by the VM. For example, "-verbose:gc,class" instructs the VM
    to print GC and class loading related messages. Standard names include:
    gc, class, and jni. All nonstandard (VM-specific) names must begin with "X".

    vfprintf    extraInfo is a pointer to the vfprintf hook.
    exit    extraInfo is a pointer to the exit hook.
    abort   extraInfo is a pointer to the abort hook.

  In addition, each VM implementation may support its own set of non-standard option
  strings. Non-standard option names must begin with "-X" or an underscore ("_").
  For example, the JDK/JRE supports -Xms and -Xmx options to allow programmers specify
  the initial and maximum heap size. Options that begin with "-X" are accessible from
  the "java" command line.

  Here is the example code that creates a Java VM in the JDK/JRE:

    JavaVMInitArgs vm_args;
    JavaVMOption options[4];

    options[0].optionString = "-Djava.compiler=NONE";           // disable JIT
    options[1].optionString = "-Djava.class.path=c:\myclasses"; // user classes
    options[2].optionString = "-Djava.library.path=c:\mylibs";  // set native library path
    options[3].optionString = "-verbose:jni";                   // print JNI-related messages

    vm_args.version = JNI_VERSION_1_2;
    vm_args.options = options;
    vm_args.nOptions = 4;
    vm_args.ignoreUnrecognized = TRUE;

  Note that in the JDK/JRE, there is no longer any need to call
    JNI_GetDefaultJavaVMInitArgs.

  res = JNI_CreateJavaVM(&vm, (void **)&env, &vm_args);
  if (res < 0) ...

  LINKAGE:
  Exported from the native library that implements the Java virtual machine.

  PARAMETERS:
    p_vm: pointer to the location where the resulting VM structure will be placed.

    p_env: pointer to the location where the JNI interface pointer for the main thread will be placed.

    vm_args: Java VM initialization arguments.

    RETURNS:
    Returns JNI_OK on success; returns a suitable JNI error code (a negative number) on failure.
 */
#include <unistd.h>
char *getExecutablePath(char * const buf, const int maxlen) {
#if 1
    size_t len = readlink("/proc/self/exe", buf, maxlen-1);
    if (len != -1) {
        buf[len] = '\0';
        return buf;
    } else {
        return NULL;
    }
#else
    //Mac OS X: _NSGetExecutablePath() (man 3 dyld)
    //Linux: readlink /proc/self/exe
    //Solaris: getexecname()
    //FreeBSD: sysctl CTL_KERN KERN_PROC KERN_PROC_PATHNAME -1
    //FreeBSD if it has procfs: readlink /proc/curproc/file (FreeBSD doesn't have procfs by default)
    //NetBSD: readlink /proc/curproc/exe
    //DragonFly BSD: readlink /proc/curproc/file
    //Windows: GetModuleFileName() with hModule = NULL
#endif
}


// Custom parameters are passed in with "-x", "-X", or "_"
static char * allocRvmCmdsForCustomCmds( const char * const p_cmd_start ) {
    char* prval = NULL;
    if (p_cmd_start == strstr(p_cmd_start, "rvm:" )) {
        // Is a custom RVM command.
        const int prmLen = strlen( p_cmd_start );
        prval = malloc(sizeof(char) * (prmLen+2));
        prval[0] = '-';
        memcpy( &prval[1], p_cmd_start, prmLen );
        prval[prmLen+1] = '\0';
    }

    return prval;
}

static char * getRVMOptionForJvmOption(const JavaVMOption* const p_opt) {
    char * p_rval = NULL;

    if (NULL == p_opt->optionString)
        return NULL;

    if (('_' == p_opt->optionString[0])) {
        // Non-standard option names must begin with "-X" or an underscore ("_"). For example, the JDK/JRE supports -Xms and -Xmx options to allow programmers specify the initial and maximum heap size. Options that begin with "-X" are accessible from the "java" command line.
        p_rval = allocRvmCmdsForCustomCmds(&p_opt->optionString[1]);
    }
    if (('-' == p_opt->optionString[0])) {
        switch (p_opt->optionString[1]) {
        case 'd':
        case 'D':
            // http://docs.oracle.com/javase/7/docs/technotes/guides/jni/spec/invocation.html
            // -D<name>=<value>	Set a system property
            fprintf(stderr, "-D option not supported in RoboVM!\n");
            break;
        case 'x':
        case 'X':
            // http://docs.oracle.com/javase/7/docs/technotes/guides/jni/spec/invocation.html
            // Non-standard option names must begin with "-X" or an underscore ("_"). For example,
            // the JDK/JRE supports -Xms and -Xmx options to allow programmers specify the initial
            // and maximum heap size. Options that begin with "-X" are accessible from the "java"
            // command line.
            p_rval = allocRvmCmdsForCustomCmds(&p_opt->optionString[2]);
            break;
        case 'v':
            if (&p_opt->optionString[1]
                    == strstr(&p_opt->optionString[1], "verbose")) {
                // http://docs.oracle.com/javase/7/docs/technotes/guides/jni/spec/invocation.html
                // -verbose[:class|gc|jni]	Enable verbose output. The options can be followed by a
                // comma-separated list of names indicating what kind of messages will be printed
                // by the VM. For example, "-verbose:gc,class" instructs the VM to print GC and
                // class loading related messages. Standard names include: gc, class, and jni. All
                // nonstandard (VM-specific) names must begin with "X".
                fprintf(stderr, "-verbose[:class|gc|jni] option not yet supported!\n");

                // Default to "debug" logging if any verbose logging is enabled.
                p_rval = allocRvmCmdsForCustomCmds("rvm:log=debug" );
            }
            break;
        default:
            break;
        }

    } else if (0 == strcmp(p_opt->optionString, "vfprintf")) {
        // extraInfo is a pointer to the vfprintf hook.
    } else if (0 == strcmp(p_opt->optionString, "exit")) {
        // extraInfo is a pointer to the exit hook.
    } else if (0 == strcmp(p_opt->optionString, "abort")) {
        // extraInfo is a pointer to the abort hook.
    }

    return p_rval;
}

static void swapCharPtrs( char **ppC0, char **ppC1 ) {
    char *tmp = *ppC0;
    *ppC0 = *ppC1;
    *ppC1 = tmp;
}

static jboolean createMainArgumentsFromVmArgs(const JavaVMInitArgs* const p_vm_args,
        int *r_argc,
		char ***r_argv) {
	jboolean ok = FALSE;
	const int MAX_PATH = 1024;

	// Arg0 to robovm always is the path to the exe
	const int n_argc_alloc = (p_vm_args) ? 1 + p_vm_args->nOptions : 1;

	// We allocate an argv for every incoming option.
	char **pp_argv = malloc(sizeof(char*) * n_argc_alloc);
	if (NULL == pp_argv) {
        fprintf(stderr, "createMainArgumentsFromVmArgs(...) failed to allocate memory!\n");
        return FALSE;
	}

	// Argv[0] points to the exe path.
	pp_argv[0] = malloc(sizeof(char) * MAX_PATH);
	pp_argv[0] = getExecutablePath(pp_argv[0], MAX_PATH);

	// Argv[1..n_argc] are converted forms of the options flags.
	if (p_vm_args) {
	    ok = TRUE;
		const int numOptions = p_vm_args->nOptions;
		for (int i = 0; i < numOptions; i++) {
			const JavaVMOption* const p_opt = &p_vm_args->options[i];
			pp_argv[1+i] = getRVMOptionForJvmOption( p_opt );
			ok &= (NULL != pp_argv[1+i]);
		}
		ok |= p_vm_args->ignoreUnrecognized;
	}

    // Move any NULL pointers to the end of pp_argv (bubble sort since it's dead simple
	// and we won't ever be passing too many parameters.)
	{
	    jboolean sorted;
	    do {
	        sorted = TRUE;
	        for (int i = 0; i < (n_argc_alloc-1); i++) {
	            if((NULL == pp_argv[i]) && (NULL != pp_argv[i+1])) {
	                swapCharPtrs( &pp_argv[i], &pp_argv[i+1] );
	                sorted = FALSE;
	            }
	        }
	    }
	    while (!sorted);
	}
    // DONE -- Move any NULL pointers to the end of pp_argv (bubble sort - meh.)

    // Count the number of contiguous parameters;
	{
        int n_argc = 0;
        while (pp_argv[n_argc]) {
            n_argc++;
        }
        if (r_argc) *r_argc = n_argc;
	}
    // DONE -- Count the number of contiguous parameters;

    if (r_argv) *r_argv = pp_argv;

	return ok;
}

void deallocArgCArgV( char ** argv ) {
    if (NULL == argv) return;
    int i = 0;
    while (argv[i]) {
        free( argv[i] );
        argv[i++] = NULL;
    }
    free( argv );
}

jint JNI_CreateJavaVM(JavaVM** p_vm, JNIEnv** p_env, void* pvm_args) {

    initOptions();

    JavaVMInitArgs *vm_args = (JavaVMInitArgs *)pvm_args;
    if (NULL != vm_args) {
        int argc = 0;
        char **argv = NULL;
        if (!createMainArgumentsFromVmArgs( vm_args, &argc, &argv ) ) {
            fprintf(stderr, "createArgCArgVFromOptions(...) failed!\n");
            deallocArgCArgV( argv );
            return 1;
        }
        if (!rvmInitOptions(argc, argv, &options, FALSE)) {
            fprintf(stderr, "rvmInitOptions(...) failed!\n");
            deallocArgCArgV( argv );
            return 1;
        }
        deallocArgCArgV( argv );
    }
    else {
        // TODO: Do with real code that maps natively, rather than adding faked argc, argv.
        const int argc = 1;
        char path[1024];
        char *argv1 = getExecutablePath(path, sizeof(path));
        char *argv[2] = {argv1, NULL};

        if (!rvmInitOptions(argc, argv, &options, FALSE)) {
            fprintf(stderr, "rvmInitOptions(...) failed!\n");
            return 1;
        }
    }

    // Start up robovm (JNI)
    Env* env = rvmStartup(&options);
    if (!env) {
        fprintf(stderr, "rvmStartup(...) failed!\n");
        return JNI_ERR;
    }

    vm = env->vm;

    // CHFO: TODO Remove!
    //rvmRunBegin(env);// ? 0 : 1;

    // Return values.
    if (p_vm) {
        *p_vm = &vm->javaVM;
    }
    if (p_env) {
        *p_env = &env->jni;
    }

    return JNI_OK;
}

/**
 Returns all Java VMs that have been created. Pointers to VMs are written in the buffer vmBuf in the order they are created. At most bufLen number of entries will be written. The total number of created VMs is returned in *nVMs.
 As of JDK/JRE 1.2, creation of multiple VMs in a single process is not supported.

 LINKAGE:
 Exported from the native library that implements the Java virtual machine.

 PARAMETERS:
 vmBuf: pointer to the buffer where the VM structures will be placed.

 bufLen: the length of the buffer.

 nVMs: a pointer to an integer.

 RETURNS:
 Returns JNI_OK on success; returns a suitable JNI error code (a negative number) on failure.
 */
jint JNI_GetCreatedJavaVMs(JavaVM** vmBuf, jsize bufLen, jsize* nVMs) {
    int numVms = (vm) ? 1 : 0;
    numVms = ( bufLen < 1 ) ? bufLen : 1;
    if ((NULL == vmBuf) || (NULL == vm)){
        return JNI_ERR;
    }
    if (bufLen >= 1) {
        *vmBuf = &vm->javaVM;
    }
    if (NULL != nVMs) {
        *nVMs = numVms;
    }
    return JNI_OK;
}


