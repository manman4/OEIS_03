M=14;

\\ n階微分
diff_n(f, x, n) = {
  my(res=f);
  for (i=1, n, res = deriv(res, x));
  return(res);
}

\\ G.f. A(x) satisfies A(x) = 1/( 1 - Sum_{k=0..6} binomial(6,k) * x^(k+1)/k! * (d^k/dx^k A(x)) ), where (d^0/dx^0 A(x)) = A(x) by convention.
my(A=1, n=M); for(i=1, n, A= 1/( 1 - sum(k=0, 6, binomial(6,k) * x^(k+1)/k! * diff_n(A, x, k)) + x*O(x^n) ) ); Vec(A)

my(A=1, n=M); for(i=1, n, A= 1/( 1 - sum(k=0, 6, binomial(6,k) * x^(k+1)/k! * diff_n(A, x, k)) + x*O(x^n)) ); Vec(1/( 1 - sum(k=0, 6, binomial(6,k) * x^(k+1)/k! * diff_n(A, x, k)) + x*O(x^n)))
