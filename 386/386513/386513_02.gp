M=14;

\\ n階微分
diff_n(f, x, n) = {
  my(res=f);
  for (i=1, n, res = deriv(res, x));
  return(res);
}

\\ E.g.f. A(x) satisfies A(x) = exp( x + x*Sum_{k=1..5} binomial(4,k-1) * x^k/k! * (d^k/dx^k A(x)) ).
my(A=1, n=M); for(i=1, n, A= exp( x + x*sum(k=1, 5, binomial(4,k-1) * x^k/k! * diff_n(A, x, k)) + x*O(x^n)) ); Vec(serlaplace(A))

my(A=1, n=M); for(i=1, n, A= exp( x + x*sum(k=1, 5, binomial(4,k-1) * x^k/k! * diff_n(A, x, k)) + x*O(x^n)) ); Vec(serlaplace(exp( x + x*sum(k=1, 5, binomial(4,k-1) * x^k/k! * diff_n(A, x, k)) + x*O(x^n)) ))
