require 'spec_helper'

#InputSource = <<-eos
#require "foo";
#def a 123;
#def b +123;
#def c -123;
#def d 321.0;
#def e +321.0;
#def f -321.0;
#def g 0b101;
#def h 0o707;
#def i 0d909;
#def j 0hF0F;
#def k 0hf0f;
#def l \\space;
#def m \\tab;
#def n \\return;
#def o \\newline;
#def p \\vtab;
#def q \\c;
#def r "";
#def s "
#";
#def t true;
#def u false;
#def v l;
#def w() 0;
#def x(a) 1;
#def y(a,b) 2;
#def z fn(a,b,c) 3;;
#
#w()
#x(1)
#y(1,2)
#z(1,2,3)
#eos
#
#ExpectedCode = <<-eos
##include "sclpl.h"
#
#_Value a;
#_Value b;
#_Value c;
#_Value d;
#_Value e;
#_Value f;
#_Value g;
#_Value h;
#_Value i;
#_Value j;
#_Value k;
#_Value l;
#_Value m;
#_Value n;
#_Value o;
#_Value p;
#_Value q;
#_Value r;
#_Value s;
#_Value t;
#_Value u;
#_Value v;
#_Value w;
#_Value x;
#_Value y;
#_Value z;
#
#static _Value fn0();
#static _Value fn1(_Value a);
#static _Value fn2(_Value a, _Value b);
#static _Value fn3(_Value a, _Value b, _Value c);
#
#static _Value fn0() {
#    return __int(0);
#}
#
#static _Value fn1(_Value a) {
#    return __int(1);
#}
#
#static _Value fn2(_Value a, _Value b) {
#    return __int(2);
#}
#
#static _Value fn3(_Value a, _Value b, _Value c) {
#    return __int(3);
#}
#
#void toplevel(void) {
#    extern void foo_toplevel(void);
#    foo_toplevel();
#    a = __int(123);
#    b = __int(123);
#    c = __int(-123);
#    d = __float(321.000000);
#    e = __float(321.000000);
#    f = __float(-321.000000);
#    g = __int(5);
#    h = __int(455);
#    i = __int(909);
#    j = __int(3855);
#    k = __int(3855);
#    l = __char(' ');
#    m = __char('\\t');
#    n = __char('\\r');
#    o = __char('\\n');
#    p = __char('\\v');
#    q = __char('c');
#    r = __string("");
#    s = __string("\\n");
#    t = __bool(true);
#    u = __bool(false);
#    v = l;
#    w = __func(&fn0);
#    x = __func(&fn1);
#    y = __func(&fn2);
#    z = __func(&fn3);
#    (void)(__call0(w));
#    (void)(__calln(x, 1, __int(1)));
#    (void)(__calln(y, 2, __int(1), __int(2)));
#    (void)(__calln(z, 3, __int(1), __int(2), __int(3)));
#}
#
#int main(int argc, char** argv) {
#    (void)argc;
#    (void)argv;
#    toplevel();
#    return 0;
#}
#eos
#
#describe "code generation" do
#  it "should generate some code" do
#    expect(ccode(InputSource)).to eq ExpectedCode
#  end
#end
