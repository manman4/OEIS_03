M=20;

\\ E.g.f. A(x) satisfies A(x) = exp(x*C(x*A(x)^2)), where C(x) = 1 + x*C(x)^2 is the g.f. of A000108.
a(n, s, t) = {
  my(A=1, B=sum(k=0, n, binomial(s*k,k)/((s-1)*k+1) * x^k) + x*O(x^(n+1))); 
  for(i=1, n, A=exp(x * subst(B, x, x*A^t) )); 
  Vec(serlaplace(A)); 
}
a(M, 2, 2)

\\ Let F(x) be the e.g.f. of A379690. F(x) = log(A(x))/x = C(x*A(x)^2).
b(n, s, t) = {
  my(A=1, B=sum(k=0, n, binomial(s*k,k)/((s-1)*k+1) * x^k) + x*O(x^(n+1))); 
  for(i=1, n, A=exp(x * subst(B, x, x*A^t) )); 
  Vec(serlaplace(subst(B, x, x*A^t))); 
}
b(M, 2, 2)