M=20;

\\ E.g.f.: F(x) = C(x*A(x)) = exp( 1/2 * Sum_{k>=1} binomial(2*k,k) * (x*A(x))^k/k ).
a(n, s, t) = {
    my(A=1, B=sum(k=0, n, binomial(s*k,k)/((s-1)*k+1) * x^k) + x*O(x^(n+1))); 
    for(i=1, n, A=exp(x + x*O(x^(n+1))) * subst(B, x, x*A^t) ); 
    Vec(serlaplace(A)); 
}
a(M, 2, 1)

\\ Let F(x) be the e.g.f. of A364983. F(x) = C(x*A(x)) = exp( 1/2 * Sum_{k>=1} binomial(2*k,k) * (x*A(x))^k/k ).
b(n, s, t) = {
    my(A=1, B=sum(k=0, n, binomial(s*k,k)/((s-1)*k+1) * x^k) + x*O(x^(n+1))); 
    for(i=1, n, A=exp(x + x*O(x^(n+1))) * subst(B, x, x*A^t) ); 
    A; 
}

g = b(M, 2, 1);
f1 = g * exp(-x + x * O(x^(M+1)));
f2 = exp( 1/2 * sum(k=1, M, binomial(2*k,k) * (x*g)^k/k) );
Vec(serlaplace(f1))
Vec(serlaplace(f2))
serlaplace(f1 - f2)
