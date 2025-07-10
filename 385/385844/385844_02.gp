M=19;

\\ n階微分
diff_n(f, x, n) = {
  my(res=f);
  for (i=1, n, res = deriv(res, x));
  return(res);
}

\\ G.f. A(x) satisfies A(x) = 1/((1 - x) * (1 - x^3*A''(x))).
my(A=1, n=M); for(i=1, n, A= 1/( (1 - x) * ( 1 - x^3*diff_n(A, x, 2)) + x*O(x^n) ) ); Vec(A)

my(A=1, n=M); for(i=1, n, A= 1/( (1 - x) * ( 1 - x^3*diff_n(A, x, 2)) + x*O(x^n) ) ); Vec(1/( (1 - x) * ( 1 - x^3*diff_n(A, x, 2)) + x*O(x^n) ) )
