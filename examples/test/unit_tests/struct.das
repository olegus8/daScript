// options log=true

struct Dummy {}

struct Ball
    position:float3
    radius:float

struct Balls
    a,b:Ball

def get_ball_radius(b:Ball):float
    return b.radius

def get_balls_radius(b:Balls):float
    return b.a.radius + b.b.radius

def set_default_ball(var b:Ball)
    b.position=float3(1.0,2.0,3.0)
    b.radius=5.0

def make_default_ball:Ball?
    var b:Ball?=new Ball
    set_default_ball(deref(b))
    return b

def verify_default_ball(b:Ball)
    assert(b.position==float3(1.0,2.0,3.0))
    assert(b.radius==5.0)

def test_ref_ptr(var a:Ball?&;b:Ball?)
    a = b
    assert(a==b)

def test_ret_ptr(a:Ball?):Ball?
    return a

struct L3
    a:int

struct L2
    l3:L3?

struct L1
    l2:L2?

var
    glob : L1? = null

struct AB
    a : uint
    b : uint

struct BA
    b : uint
    a : uint

struct A
    a : uint

def finalize(var s : AB)
    pass

def finalize(var s : BA)
    pass

def finalize(var s : A)
    pass

def test_finalizers()
    var ab : AB

    /*
    30304: too many custom  finalize functions exist for finalize ( s : AB
    -const ) : void
    candidates are:
        finalize ( s : AB -const ) : void
        finalize ( s : A -const ) : void
    */
    //delete ab


    var ba : BA
    delete ba
    var a : A
    delete a

[export]
def test:bool
    test_finalizers()
    assert ( typeinfo(sizeof [[Dummy]])==0 )
    if true
        var b:Ball? = new Ball
        b.position = float3(1.0,2.0,3.0)
        b.radius = 5.0
        verify_default_ball(deref(b))
        if true
            var xb, qb:Ball
            var qc:Ball? = b
            var qd:Ball? = new Ball
            qb = deref(qc)
            verify_default_ball(qb)
            verify_default_ball(deref(qc))
            qc = null
            assert(qc==null)
            assert(qd!=null)
            test_ref_ptr(qc,qd)
            assert(qc==qd)
            qd = test_ret_ptr(null)
            assert(qd==null)
            assert(qd!=qc)
            xb = qb
            verify_default_ball(xb)
    if true
        let b:Ball? = make_default_ball()
        verify_default_ball(deref(b))
    if true
        var dummy:int
        glob?.l2?.l3?.a ?? dummy = 1
        assert(dummy==1)
        glob = new L1
        glob?.l2?.l3?.a ?? dummy = 2
        assert(dummy==2)
        glob.l2 = new L2
        glob?.l2?.l3?.a ?? dummy = 3
        assert(dummy==3)
        glob.l2.l3 = new L3
        glob?.l2?.l3?.a ?? dummy = 4
        assert(dummy==3)
        assert(glob?.l2?.l3?.a ?? 5==4)
    return true

