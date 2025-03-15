M=20;

\\ E.g.f. A(x) satisfies A(x) = exp(x*A(x) * B(x*A(x))^3).
a(n, s, t) = {
    my(A=1, B=sum(k=0, n, binomial(s*k,k)/((s-1)*k+1) * x^k) + x*O(x^(n+1))); 
    for(i=1, n, A=exp(x * A * subst(B, x, x*A)^t )); 
    Vec(serlaplace(A)); 
}
a(M, 4, 3)