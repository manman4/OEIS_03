M=15;

\\ n階微分
diff_n(f, x, n) = {
  my(res=f);
  for (i=1, n, res = deriv(res, x));
  return(res);
}

\\ G.f. A(x) satisfies A(x) = 1/( 1 - x - x*Sum_{k=1..5} binomial(4,k-1) * x^k/k! * (d^k/dx^k A(x)) ).
my(A=1, n=M); for(i=1, n, A= 1/( 1 - x - x*sum(k=1, 5, binomial(4,k-1) * x^k/k! * diff_n(A, x, k)) + x*O(x^n)) ); Vec(A)

my(A=1, n=M); for(i=1, n, A= 1/( 1 - x - x*sum(k=1, 5, binomial(4,k-1) * x^k/k! * diff_n(A, x, k)) + x*O(x^n)) ); Vec(1/( 1 - x - x*sum(k=1, 5, binomial(4,k-1) * x^k/k! * diff_n(A, x, k)) + x*O(x^n)) )
