module jni

type Void = bool
type Type = JavaObject | Void | bool | f32 | f64| i16 | int | i64 | string //| rune |  byte

// pub type Any = string | int | i64 | f32 | f64 | bool | []Any | map[voidptr]Any
enum MethodType {
	@static
	instance
}

/*
Signature table

Signature                 | Java Type             | V Type
---------------------------------------------------------------------------
Z                         | boolean               | bool
---------------------------------------------------------------------------
B                         | byte                  | byte
---------------------------------------------------------------------------
C                         | char                  | rune
---------------------------------------------------------------------------
S                         | short                 | i16
---------------------------------------------------------------------------
I                         | int                   | int
---------------------------------------------------------------------------
J                         | long                  | i64
---------------------------------------------------------------------------
F                         | float                 | f32
---------------------------------------------------------------------------
D                         | double                | f64
---------------------------------------------------------------------------
L fully-qualified-class ; | fully-qualified-class | N/A
---------------------------------------------------------------------------
[ type                    | type[]                |
---------------------------------------------------------------------------
( arg-types ) ret-type    | method type           |
---------------------------------------------------------------------------
*/
pub fn v2j_fn_name(v_func_name string) string {
	splt := v_func_name.split('_')
	mut java_fn_name := splt[0]
	for i := 1; i < splt.len; i++ {
		java_fn_name += splt[i].capitalize()
	}
	return java_fn_name
}

fn v2j_signature_type(vt Type) string {
	return match vt {
		bool { 'Z' }
		f32 { 'F' }
		f64 { 'D' }
		i16 { 'S' }
		int { 'I' }
		i64 { 'J' }
		string { 'Ljava/lang/String;' }
		JavaObject { 'Ljava/lang/Object;' }
		else { 'V' } // void
	}
}

fn v2j_string_signature_type(vt string) string {
	return match vt {
		'bool' { 'Z' }
		'i16' { 'S' }
		'int' { 'I' }
		'i64' { 'J' }
		'f32' { 'F' }
		'f64' { 'D' }
		'string' { 'Ljava/lang/String;' }
		'object' { 'Ljava/lang/Object;' }
		else { 'V' } // void
	}
}

fn v2j_value(vt Type) JavaValue {
	return match vt {
		bool {
			JavaValue{
				z: jboolean(vt)
			}
		}
		f32 {
			JavaValue{
				i: jfloat(vt)
			}
		}
		f64 {
			JavaValue{
				i: jdouble(vt)
			}
		}
		i16 {
			JavaValue{
				s: jshort(vt)
			}
		}
		int {
			JavaValue{
				i: jint(vt)
			}
		}
		i64 {
			JavaValue{
				j: jlong(vt)
			}
		}
		string {
			// TODO this assumes default env
			jstr := jstring(default_env(), vt)
			jobj := &JavaObject(voidptr(&jstr))
			JavaValue{
				l: jobj
			}
			//JavaValue{
			//	l: C.StringToObject(jstring(default_env(), vt))
			//}
		}
		JavaObject {
			JavaValue{
				l: vt //JavaObject(vt)
			}
		}
		else {
			JavaValue{}
		}
	}
}

[inline]
pub fn v2j_signature(fqn_signature string) (string, string, string) {
	clazz := fqn_signature.all_before_last('.')
	fn_sig := fqn_signature.all_after_last('.')
	f_name := fn_sig.all_before('(')
	f_sig := '(' + fn_sig.all_after('(')
	return clazz, f_name, f_sig
}

[inline]
pub fn j2v_string(env &Env, jstr JavaString) string {
	mut cn := ''
	unsafe {
		native_string := C.GetStringUTFChars(env, jstr, C.jboolean(C.JNI_FALSE))
		cn = '' + native_string.vstring()
		C.ReleaseStringUTFChars(env, jstr, native_string)
	}
	return cn
}

[inline]
pub fn j2v_char(jchar C.jchar) rune {
	return rune(u16(jchar))
}

[inline]
pub fn j2v_byte(jbyte C.jbyte) byte {
	return byte(i8(jbyte))
}

[inline]
pub fn j2v_boolean(jbool C.jboolean) bool {
	return jbool == C.jboolean(C.JNI_TRUE)
}

[inline]
pub fn j2v_int(jint C.jint) int {
	return int(jint)
}


[inline]
pub fn j2v_size(jsize C.jsize) int {
	return int(jsize)
}

[inline]
pub fn j2v_short(jshort C.jshort) i16 {
	return i16(jshort)
}

[inline]
pub fn j2v_long(jlong C.jlong) i64 {
	return i64(jlong)
}

[inline]
pub fn j2v_float(jfloat C.jfloat) f32 {
	return f32(jfloat)
}

[inline]
pub fn j2v_double(jdouble C.jdouble) f64 {
	return f64(jdouble)
}

[inline]
pub fn jboolean(val bool) C.jboolean {
	if val {
		return C.jboolean(C.JNI_TRUE)
	}
	return C.jboolean(C.JNI_FALSE)
}

[inline]
pub fn jbyte(val byte) C.jbyte {
	return C.jbyte(val)
}

[inline]
pub fn jfloat(val f32) C.jfloat {
	return C.jfloat(val)
}

[inline]
pub fn jdouble(val f64) C.jdouble {
	return C.jdouble(val)
}

[inline]
pub fn jint(val int) C.jint {
	return C.jint(val)
}

[inline]
pub fn jsize(val int) C.jsize {
	return C.jsize(val)
}

[inline]
pub fn jshort(val i16) C.jshort {
	return C.jshort(val)
}


[inline]
pub fn jlong(val i64) C.jlong {
	return C.jlong(val)
}

[inline]
pub fn jchar(val rune) C.jchar {
	return C.jchar(val)
}

[inline]
pub fn jstring(env &Env, val string) C.jstring {
	return C.NewStringUTF(env, val.str)
}
