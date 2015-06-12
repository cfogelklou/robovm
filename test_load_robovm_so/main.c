#include <stdio.h>
#include <dlfcn.h>
//#include <robovm.h>
#include <jni.h>
#include <assert.h>



int main( void ) {
	jint (*pFnJNI_CreateJavaVM)(JavaVM** p_vm, JNIEnv** p_env, void* pvm_args);
	void * pLib = dlopen("/tmp/robovm_java.so", RTLD_NOW);
	if (NULL == pLib) {
		char *pErr = dlerror();
		printf("%s\n", pErr);
		return -1;
	}
	pFnJNI_CreateJavaVM =dlsym( pLib, "JNI_CreateJavaVM");
	if (NULL == pFnJNI_CreateJavaVM) {
		pFnJNI_CreateJavaVM =dlsym( pLib, "_JNI_CreateJavaVM");
		if (NULL == pFnJNI_CreateJavaVM) {
			char *pErr = dlerror();
			printf("%s\n", pErr);
			dlclose( pLib );
			return -1;
		}
	}

	{
		jint res;
	    JavaVM *jvm = NULL;
	    JNIEnv *env;       /* pointer to native method interface */
	    {
	        JavaVMInitArgs vm_args;
	        JavaVMOption options[] = {
	            {
	                "-Djava.compiler=NONE",           /* disable JIT */
	                NULL
	            },
	            {
	                "-Djava.class.path=c:\\myclasses", /* user classes */
	                NULL
	            },
	            {
	                "-Djava.library.path=c:\\mylibs",  /* set native library path */
	                NULL
	            },
	            {
	                "-xrvm:log=debug",
	                NULL
	            },
	            {
	                "-verbose:jni",                   /* print JNI-related messages */
	                NULL
	            }
	        };

	        vm_args.version = JNI_VERSION_1_2;
	        vm_args.options = options;
	        vm_args.nOptions = sizeof(options)/sizeof(options[0]);
	        vm_args.ignoreUnrecognized = 1;

	        /* Note that in the JDK/JRE, there is no longer any
	         * need to call JNI_GetDefaultJavaVMInitArgs().
	         */
	        res = pFnJNI_CreateJavaVM(&jvm, &env, &vm_args);

	        assert( JNI_OK == res );

	        /* invoke the Main.test method using the JNI */
	        jclass cls = (*env)->FindClass(env, "TestLib");

	        jmethodID mid = (*env)->GetStaticMethodID(env, cls, "hello", "([Ljava/lang/String;)V");

	        (*env)->CallStaticVoidMethod(env, cls, mid, NULL);

	        /* We are done. */
	        (*jvm)->DestroyJavaVM( jvm );
	    }

	}


	dlclose(pLib);

	return 0;
}
