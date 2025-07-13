M=16;

\\ n階微分
diff_n(f, x, n) = {
  my(res=f);
  for (i=1, n, res = deriv(res, x));
  return(res);
}

\\ .g.f. A(x) satisfies A(x) = exp( Sum_{k=0..2} binomial(2,k) * x^(k+1)/(k+1)! * (d^k/dx^k A(x)) ), where (d^0/dx^0 A(x)) = A(x) by convention.
my(A=1, n=M); for(i=1, n, A= exp( sum(k=0, 2, binomial(2,k) * x^(k+1)/(k+1)! * diff_n(A, x, k)) + x*O(x^n) ) ); Vec(serlaplace(A))

my(A=1, n=M); for(i=1, n, A= exp( sum(k=0, 2, binomial(2,k) * x^(k+1)/(k+1)! * diff_n(A, x, k)) + x*O(x^n)) ); Vec(serlaplace(exp( sum(k=0, 2, binomial(2,k) * x^(k+1)/(k+1)! * diff_n(A, x, k)) + x*O(x^n))))
