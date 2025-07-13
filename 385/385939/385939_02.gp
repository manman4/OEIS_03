M=15;

\\ n階微分
diff_n(f, x, n) = {
  my(res=f);
  for (i=1, n, res = deriv(res, x));
  return(res);
}

\\ E.g.f. A(x) satisfies A(x) = exp( x * A(x) + x^2 * (d/dx A(x)) + x^3 * (d^2/dx^2 A(x)) ).
my(A=1, n=M); for(i=1, n, A= exp( x*A + x^2 * diff_n(A, x, 1) + x^3 * diff_n(A, x, 2) + x*O(x^n) ) ); Vec(serlaplace(A))

my(A=1, n=M); for(i=1, n, A= exp( x*A + x^2 * diff_n(A, x, 1) + x^3 * diff_n(A, x, 2) + x*O(x^n)) ); Vec(serlaplace(exp( x*A + x^2 * diff_n(A, x, 1) + x^3 * diff_n(A, x, 2) + x*O(x^n))))
