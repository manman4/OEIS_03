\\ a(n) = (2*n-1)! * [x^(2*n-1)] (S(x)'' - 2*x*S(x)' - 2*S(x)) / 2, where e.g.f. S(x) satisfies S(x) = Integral (1 + S(x)^2)^2 dx.
a(n) = my(S=x); S = serreverse( intformal( 1/(1 + x^2 +x*O(x^(2*n)))^2)); (2*n-1)!*polcoef((deriv(deriv(S)) - 2*x*deriv(S) - 2*S) / 2, 2*n-1);
for(n=1, 30, print1(a(n), ", "));

a(n) = my(S=x); S = serreverse( intformal( 1/(1 + x^2 +x*O(x^(2*n)))^2)); S;
a(10)

a(n) = my(S=x); S = serreverse( intformal( 1/(1 + x^2 +x*O(x^(2*n)))^2)); (deriv(deriv(S)) - 2*x*deriv(S) - 2*S) / 2;
a(10)
