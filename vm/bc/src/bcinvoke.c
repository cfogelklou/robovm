
#include <robovm.h>
//#include "uthash.h"
//#include "utlist.h"
//#include "MurmurHash3.h"
//#include "classinfo.h"
#include <jni.h>       /* where everything is defined */
#include <jni_types.h>

int main(int argc, char *argv[]) {
    jint res;
    JavaVM *jvm = NULL;
    JNIEnv *env;       /* pointer to native method interface */
    {
        JavaVMInitArgs vm_args;
        JavaVMOption options[5];

        options[0].optionString = "-Djava.compiler=NONE";           /* disable JIT */
        options[1].optionString = "-Djava.class.path=c:\\myclasses"; /* user classes */
        options[2].optionString = "-Djava.library.path=c:\\mylibs";  /* set native library path */
        options[3].optionString = "-xrvm:log=debug";
        options[4].optionString = "-verbose:jni";                   /* print JNI-related messages */

        vm_args.version = JNI_VERSION_1_2;
        vm_args.options = options;
        vm_args.nOptions = sizeof(options)/sizeof(options[0]);
        vm_args.ignoreUnrecognized = TRUE;

        /* Note that in the JDK/JRE, there is no longer any need to call
         * JNI_GetDefaultJavaVMInitArgs.
         */
        //fprintf(stderr, "Creating JNI via JNI_CreateJavaVM()");
        res = JNI_CreateJavaVM(&jvm, &env, &vm_args);

        assert( JNI_OK == res );

        /* invoke the Main.test method using the JNI */
        //jclass cls = env->FindClass("Main");
        jclass cls = (*env)->FindClass(env, "HelloWorld");
        //jmethodID mid = env->GetStaticMethodID(cls, "test", "(I)V");
        jmethodID mid = (*env)->GetStaticMethodID(env, cls, "main", "([Ljava/lang/String;)V");
        //env->CallStaticVoidMethod(cls, mid, 100);
        (*env)->CallStaticVoidMethod(env, cls, mid, NULL);
        /* We are done. */
        (*jvm)->DestroyJavaVM( jvm );
    }

}

