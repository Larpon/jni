// Copyright(C) 2021 Lars Pontoppidan. All rights reserved.
// Use of this source code is governed by an MIT license file distributed with this software package
#include <stdarg.h>
#include <stdio.h>

#ifdef __ANDROID__
#define V_JNI_ANDROID_LOG_TAG "V_ANDROID"
#define V_JNI_ANDROID_LOG_I(...) __android_log_print(ANDROID_LOG_INFO, V_JNI_ANDROID_LOG_TAG, __VA_ARGS__)
#define V_JNI_ANDROID_LOG_W(...) __android_log_print(ANDROID_LOG_WARN, V_JNI_ANDROID_LOG_TAG, __VA_ARGS__)
#define V_JNI_ANDROID_LOG_E(...) __android_log_print(ANDROID_LOG_ERROR, V_JNI_ANDROID_LOG_TAG, __VA_ARGS__)
#define V_JNI_ANDROID_LOG_D(...) __android_log_print(ANDROID_LOG_DEBUG, V_JNI_ANDROID_LOG_TAG, __VA_ARGS__)
#endif

static JavaVM* gJavaVM;

static jobject gClassLoader;
static jmethodID gFindClassMethod;

void __v_jni_log_i(const char *fmt, ...) {
	va_list args;
    va_start(args, fmt);
	#ifdef __ANDROID__
		V_JNI_ANDROID_LOG_I(fmt, args);
	#else
		vprintf(fmt, args);
	#endif
	va_end(args);
}

void __v_jni_log_w(const char *fmt, ...) {
	va_list args;
    va_start(args, fmt);
	#ifdef __ANDROID__
		V_JNI_ANDROID_LOG_W(fmt, args);
	#else
		vprintf(fmt, args);
	#endif
	va_end(args);
}

void __v_jni_log_e(const char *fmt, ...) {
	va_list args;
    va_start(args, fmt);
	#ifdef __ANDROID__
		V_JNI_ANDROID_LOG_E(fmt, args);
	#else
		vprintf(fmt, args);
	#endif
	va_end(args);
}

void __v_jni_log_d(const char *fmt, ...) {
	#if defined(_VDEBUG)
	{
		va_list args;
		va_start(args, fmt);
		#ifdef __ANDROID__
			V_JNI_ANDROID_LOG_D(fmt, args);
		#else
			vprintf(fmt, args);
		#endif
		va_end(args);
	}
	#endif
}

void gSetJavaVM(JavaVM* vm) {
	__v_jni_log_d("jni.c.gSetJavaVM %p\n", vm);
	gJavaVM = vm;
}

JavaVM* gGetJavaVM() {
	return gJavaVM;
}

// Utility function to get JNIEnv
JNIEnv* gGetEnv() {
	JNIEnv *env;
	if (gJavaVM == 0) {
		__v_jni_log_e("jni.c: Invalid global Java VM");
		return 0;
	}

	int status;
	status = (*gJavaVM)->GetEnv(gJavaVM,(void **) &env, JNI_VERSION_1_6);

	if (status < 0) {
		__v_jni_log_i("jni.c: Attaching thread to get JNI environment from Java VM %p", gJavaVM);
		// Try to attach native thread to JVM:
		status = (*gJavaVM)->AttachCurrentThread(gJavaVM, &env, 0);
		if (status < 0) {
			__v_jni_log_e("jni.c: Failed to attach current thread to Java VM %p", gJavaVM);
			return 0;
		}
		__v_jni_log_i("jni.c: Attached to thread successfully");
	}

	return env;
}

void gSetupAndroid(const char *fqActivityName) {
	#ifdef __ANDROID__
	__v_jni_log_d("jni.c.gSetupAndroid()");
	__v_jni_log_d(fqActivityName);
	JNIEnv *env = gGetEnv();
	//replace with one of your classes in the line below
	__v_jni_log_d("jni.c.gSetupAndroid() finding activity class...");
	jclass randomClass = (*env)->FindClass(env, fqActivityName);
	__v_jni_log_d("jni.c.gSetupAndroid() FindClass %p", randomClass);
	if (ExceptionCheck(env) == JNI_TRUE) { ExceptionDescribe(env); }
	jclass classClass = (*env)->GetObjectClass(env, randomClass);
	__v_jni_log_d("jni.c.gSetupAndroid() GetObjectClass %p", classClass);
	if (ExceptionCheck(env) == JNI_TRUE) { ExceptionDescribe(env); }
	jclass classLoaderClass = (*env)->FindClass(env, "java/lang/ClassLoader");
	__v_jni_log_d("jni.c.gSetupAndroid() FindClass %p", classLoaderClass);
	if (ExceptionCheck(env) == JNI_TRUE) { ExceptionDescribe(env); }
	jmethodID getClassLoaderMethod = (*env)->GetMethodID(env, classClass, "getClassLoader", "()Ljava/lang/ClassLoader;");
	__v_jni_log_d("jni.c.gSetupAndroid() GetMethodID %d", getClassLoaderMethod);
	if (ExceptionCheck(env) == JNI_TRUE) { ExceptionDescribe(env); }
	gClassLoader = (*env)->NewGlobalRef(env, (*env)->CallObjectMethod(env, randomClass, getClassLoaderMethod));
	__v_jni_log_d("jni.c.gSetupAndroid() gClassLoader %p", gClassLoader);
	if (ExceptionCheck(env) == JNI_TRUE) { ExceptionDescribe(env); }
	gFindClassMethod = (*env)->GetMethodID(env, classLoaderClass, "findClass", "(Ljava/lang/String;)Ljava/lang/Class;");
	__v_jni_log_d("jni.c.gSetupAndroid() GetMethodID %d", gFindClassMethod);
	if (ExceptionCheck(env) == JNI_TRUE) { ExceptionDescribe(env); }
	#endif
}

/*
// Called to save JavaVM if library is loaded from Java:
int JNI_OnLoad(JavaVM* vm, void* reserved) {
	__v_jni_log_i("JNI_OnLoad: saving gJavaVM %p",vm);
	gSetJavaVM(vm);

	#ifdef __ANDROID__
	JNIEnv *env = gGetEnv();
	//replace with one of your classes in the line below
	jclass randomClass = (*env)->FindClass(env, "io/v/android/VActivity");
	__v_jni_log_i("FindClass %p", randomClass);
	if (ExceptionCheck(env) == JNI_TRUE) { ExceptionDescribe(env); }
	jclass classClass = (*env)->GetObjectClass(env, randomClass);
	__v_jni_log_i("GetObjectClass %p", classClass);
	if (ExceptionCheck(env) == JNI_TRUE) { ExceptionDescribe(env); }
	jclass classLoaderClass = (*env)->FindClass(env, "java/lang/ClassLoader");
	__v_jni_log_i("FindClass %p", classLoaderClass);
	if (ExceptionCheck(env) == JNI_TRUE) { ExceptionDescribe(env); }
	jmethodID getClassLoaderMethod = (*env)->GetMethodID(env, classClass, "getClassLoader", "()Ljava/lang/ClassLoader;");
	__v_jni_log_i("GetMethodID %d", getClassLoaderMethod);
	if (ExceptionCheck(env) == JNI_TRUE) { ExceptionDescribe(env); }
	gClassLoader = (*env)->NewGlobalRef(env, (*env)->CallObjectMethod(env, randomClass, getClassLoaderMethod));
	__v_jni_log_i("gClassLoader %p", gClassLoader);
	if (ExceptionCheck(env) == JNI_TRUE) { ExceptionDescribe(env); }
	gFindClassMethod = (*env)->GetMethodID(env, classLoaderClass, "findClass", "(Ljava/lang/String;)Ljava/lang/Class;");
	__v_jni_log_i("GetMethodID %d", gFindClassMethod);
	if (ExceptionCheck(env) == JNI_TRUE) { ExceptionDescribe(env); }
	#endif

	return JNI_VERSION_1_6;
}*/

jclass gFindClass(const char *name) {
	JNIEnv *env = gGetEnv();
	return (*env)->CallObjectMethod(env, gClassLoader, gFindClassMethod, (*env)->NewStringUTF(env, name));
}

//long VoidToLong(void* vp) {
//    return (long)vp;
//}

//void* LongToVoid(long l) {
//    return (void*)l;
//}

//jobject StringToObject(jstring str) {
//    return (jobject)str;
//}

//jstring ObjectToString(jobject obj) {
//    return (jstring)obj;
//}

//jclass ObjectToClass(jobject obj) {
//    return (jclass)obj;
//}

//jobject ClassToObject(jclass cls) {
//    return (jobject)cls;
//}

//jobject MethodIDToObject(jmethodID meth) {
//	return (jobject)meth;
//}
