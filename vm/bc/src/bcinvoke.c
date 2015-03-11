
#include <robovm.h>
#include "uthash.h"
#include "utlist.h"
#include "MurmurHash3.h"
#include "classinfo.h"
#include <jni.h>       /* where everything is defined */

int main(int argc, char *argv[]) {
    jint res;
    JavaVM *vm = NULL;
    JNIEnv *env;       /* pointer to native method interface */
    {
        JavaVMInitArgs vm_args;
        JavaVMOption options[4];

        options[0].optionString = "-Djava.compiler=NONE";           /* disable JIT */
        options[1].optionString = "-Djava.class.path=c:\\myclasses"; /* user classes */
        options[2].optionString = "-Djava.library.path=c:\\mylibs";  /* set native library path */
        options[3].optionString = "-verbose:jni";                   /* print JNI-related messages */

        vm_args.version = JNI_VERSION_1_2;
        vm_args.options = options;
        vm_args.nOptions = 4;
        vm_args.ignoreUnrecognized = TRUE;

        /* Note that in the JDK/JRE, there is no longer any need to call
         * JNI_GetDefaultJavaVMInitArgs.
         */
        fprintf(stderr, "Creating JNI via JNI_CreateJavaVM()");
        res = JNI_CreateJavaVM(&vm, &env, &vm_args);
    }

}

