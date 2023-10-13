b(n, k) = sumdiv(n, d, moebius(n/d)*d^k)/eulerphi(n);
T(n, k) = sumdiv(n, d, b(d, k));

f(n) = eulerphi(n);

a5(n)  = sumdiv(n, i, sumdiv(n, j, sumdiv(n, k, sumdiv(n, l, sumdiv(n, m,                                                                                            f(i)*f(j)*f(k)*f(l)*f(m)                                   /f(lcm([i, j, k, l, m]                     )))))));
a6(n)  = sumdiv(n, i, sumdiv(n, j, sumdiv(n, k, sumdiv(n, l, sumdiv(n, m, sumdiv(n, a,                                                                               f(i)*f(j)*f(k)*f(l)*f(m)*f(a)                              /f(lcm([i, j, k, l, m, a]                  ))))))));
a7(n)  = sumdiv(n, i, sumdiv(n, j, sumdiv(n, k, sumdiv(n, l, sumdiv(n, m, sumdiv(n, a, sumdiv(n, b,                                                                  f(i)*f(j)*f(k)*f(l)*f(m)*f(a)*f(b)                         /f(lcm([i, j, k, l, m, a, b]               )))))))));
a8(n)  = sumdiv(n, i, sumdiv(n, j, sumdiv(n, k, sumdiv(n, l, sumdiv(n, m, sumdiv(n, a, sumdiv(n, b, sumdiv(n, c,                                                     f(i)*f(j)*f(k)*f(l)*f(m)*f(a)*f(b)*f(c)                    /f(lcm([i, j, k, l, m, a, b, c]            ))))))))));
a9(n)  = sumdiv(n, i, sumdiv(n, j, sumdiv(n, k, sumdiv(n, l, sumdiv(n, m, sumdiv(n, a, sumdiv(n, b, sumdiv(n, c, sumdiv(n, d,                                        f(i)*f(j)*f(k)*f(l)*f(m)*f(a)*f(b)*f(c)*f(d)               /f(lcm([i, j, k, l, m, a, b, c, d]         )))))))))));
a10(n) = sumdiv(n, i, sumdiv(n, j, sumdiv(n, k, sumdiv(n, l, sumdiv(n, m, sumdiv(n, a, sumdiv(n, b, sumdiv(n, c, sumdiv(n, d, sumdiv(n, e,                           f(i)*f(j)*f(k)*f(l)*f(m)*f(a)*f(b)*f(c)*f(d)*f(e)          /f(lcm([i, j, k, l, m, a, b, c, d, e]      ))))))))))));
a11(n) = sumdiv(n, i, sumdiv(n, j, sumdiv(n, k, sumdiv(n, l, sumdiv(n, m, sumdiv(n, a, sumdiv(n, b, sumdiv(n, c, sumdiv(n, d, sumdiv(n, e, sumdiv(n, g,              f(i)*f(j)*f(k)*f(l)*f(m)*f(a)*f(b)*f(c)*f(d)*f(e)*f(g)     /f(lcm([i, j, k, l, m, a, b, c, d, e, g]   )))))))))))));
a12(n) = sumdiv(n, i, sumdiv(n, j, sumdiv(n, k, sumdiv(n, l, sumdiv(n, m, sumdiv(n, a, sumdiv(n, b, sumdiv(n, c, sumdiv(n, d, sumdiv(n, e, sumdiv(n, g, sumdiv(n, h, f(i)*f(j)*f(k)*f(l)*f(m)*f(a)*f(b)*f(c)*f(d)*f(e)*f(g)*f(h)/f(lcm([i, j, k, l, m, a, b, c, d, e, g, h]))))))))))))));

for(n=1, 20, print1(a5(n) ==T(n, 5), ", "));
for(n=1, 20, print1(a6(n) ==T(n, 6), ", "));
for(n=1, 20, print1(a7(n) ==T(n, 7), ", "));
for(n=1, 20, print1(a8(n) ==T(n, 8), ", "));
for(n=1, 10, print1(a9(n) ==T(n, 9), ", "));
for(n=1, 10, print1(a10(n)==T(n, 10),", "));
for(n=1, 10, print1(a11(n)==T(n, 11),", "));
for(n=1, 10, print1(a12(n)==T(n, 12),", "));

for(n=1, 19, print1(T(n, n),", "));