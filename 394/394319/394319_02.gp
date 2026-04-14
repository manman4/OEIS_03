\\ a(n) = (2*n)! * [x^(2*n)] (C(x)'' - 2*x*C(x)' - C(x)) / 2, where e.g.f. C(x) satisfies C(x) = cosh( Integral C(x)^3 dx ).
a(n) = my(S=x, C=1); for(i=0, n, S = intformal( C^4 +x*O(x^(2*n))); C = 1 + intformal( S*C^3 ) ); (deriv(deriv(C))-2*x*deriv(C)-C)/2;
v=a(215);
for(n=0, 215, write("b394319.txt", n, " ", (2*n)! * polcoef(v, 2*n)));