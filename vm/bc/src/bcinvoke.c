
#include <robovm.h>
//#include <jni.h>       /* where everything is defined */
//#include <jni_types.h>

int main(int argc, char *argv[]) {
    jint res;
    JavaVM *jvm = NULL;
    JNIEnv *env;       /* pointer to native method interface */
    {
        JavaVMInitArgs vm_args;
        JavaVMOption options[] = {
            {
                "-xrvm:log=debug",
                NULL
            },
            {
                "-xrvm:exepath=/home/appsupport/Projects/robovm/robovm/test_load_robovm_so/",
                NULL
            },
        };

        vm_args.version = JNI_VERSION_1_2;
        vm_args.options = options;
        vm_args.nOptions = sizeof(options)/sizeof(options[0]);
        vm_args.ignoreUnrecognized = TRUE;

        /* Note that in the JDK/JRE, there is no longer any
         * need to call JNI_GetDefaultJavaVMInitArgs().
         */
        res = JNI_CreateJavaVM(&jvm, &env, &vm_args);

        assert( JNI_OK == res );

        /* invoke the Main.test method using the JNI */
        jclass cls = (*env)->FindClass(env, "HelloWorld");

        jmethodID mid = (*env)->GetStaticMethodID(env, cls, "main", "([Ljava/lang/String;)V");

        (*env)->CallStaticVoidMethod(env, cls, mid, NULL);

        /* We are done. */
        (*jvm)->DestroyJavaVM( jvm );
    }

}

