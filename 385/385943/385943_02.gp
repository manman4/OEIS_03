M=11;

\\ n階微分
diff_n(f, x, n) = {
  my(res=f);
  for (i=1, n, res = deriv(res, x));
  return(res);
}

\\ E.g.f. A(x) satisfies A(x) = exp( x*A(x) + x*Sum_{k=1..6} Stirling2(6,k) * x^k * (d^k/dx^k A(x)) ).
my(A=1, n=M); for(i=1, n, A= exp( x*A + x*sum(k=1, 6, stirling(6, k, 2) * x^k * diff_n(A, x, k)) + x*O(x^n) ) ); Vec(serlaplace(A))

my(A=1, n=M); for(i=1, n, A= exp( x*A + x*sum(k=1, 6, stirling(6, k, 2) * x^k * diff_n(A, x, k)) + x*O(x^n)) ); Vec(serlaplace(exp( x*A + x*sum(k=1, 6, stirling(6, k, 2) * x^k * diff_n(A, x, k)) + x*O(x^n))))
