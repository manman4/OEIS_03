\\ A370686 from the algebraic equation
\\ (x^4+16*x^3+2*x^2-3)*A(x)^4 + (4*x^3+4*x)*A(x)^3
\\ + (6*x^2+2)*A(x)^2 + 4*x*A(x) + 1 = 0,
\\ where A(x) = Sum_{n>=0} a(n)*x^n and a(0) = 1.
\\
\\ If we temporarily truncate A(x) before x^n, then the coefficient of x^n
\\ in the quartic is c_known - 8*a(n). Hence a(n) = c_known/8.

a_vec(N)=
{
  my(v = vector(N + 1), x = 'x);
  v[1] = 1;
  for(n = 1, N,
    my(A = sum(k = 0, n - 1, v[k + 1]*x^k) + O(x^(n + 1)));
    my(c = polcoeff((x^4 + 16*x^3 + 2*x^2 - 3)*A^4
                  + (4*x^3 + 4*x)*A^3
                  + (6*x^2 + 2)*A^2
                  + 4*x*A
                  + 1, n));
    if(c % 8, error("nonintegral step at n = ", n));
    v[n + 1] = c / 8;
  );
  v
};

a(n)=
{
  if(n < 0, error("n must be nonnegative"));
  a_vec(n)
};
print(a(30));
