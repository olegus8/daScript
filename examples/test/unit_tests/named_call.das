// options log = true, log_infer_passes = true

require daslib/safe_addr

def boo(a:int=1;b:int)
    return a + b

def boo(a,b,c:int)
    return a + b + c

def test_function_infer
    let t1 = boo([a=2,b=3])
    assert(t1==5)
    let t2 = boo([b=3])
    assert(t2==4)
    let t3 = boo([a=1,b=2,c=3])
    assert(t3==6)

def hoo(a=1;b)
    return a + b

def hoo(a,b,c)
    return a + b + c

def fn_with_default_null_ptr(var a : int? implicit = null)
    var b : int
    a ?? b = 2

def test_generic_infer
    let t1 = hoo([a=2,b=3])
    assert(t1==5)
    let t2 = hoo([b=3])
    assert(t2==4)
    let t3 = hoo([a=1,b=2,c=3])
    assert(t3==6)

def test_default_ptr
    var a : int = 1
    //TODO: this fails saying types mismatch int? and void?
    //fn_with_default_null_ptr()
    assert(a == 1)
    fn_with_default_null_ptr(safe_addr(a))
    assert(a == 2)

[export]
def test
    test_function_infer()
    test_generic_infer()
    test_default_ptr()
    return true


