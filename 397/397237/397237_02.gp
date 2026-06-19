\\ Proof. Let
\\ G(x) = 1 / ( P(x^s) * Q(x^s)^(s-1-k) * R(x^s)^k * ( Q(x^s) - x^m * R(x^s) ) )
\\     = Sum_{n>=0} a(n) * x^n,
\\ where s >= 2, m >= 1, 0 <= k <= s-1, gcd(m,s)=1, and P(0)Q(0)R(0) != 0.
\\ Multiply numerator and denominator by
\\ Sum_{j=0}^{s-1} Q(x^s)^j * (x^m * R(x^s))^(s-1-j).
\\ Using A^s - B^s = (A-B) * Sum_{j=0}^{s-1} A^j * B^(s-1-j), with
\\ A = Q(x^s), B = x^m * R(x^s), we get
\\ G(x)
\\ = Sum_{j=0}^{s-1}
\\   x^(m*(s-1-j))
\\   * Q(x^s)^j * R(x^s)^(s-1-j)
\\   / ( P(x^s) * Q(x^s)^(s-1-k) * R(x^s)^k
\\       * ( Q(x^s)^s - x^(m*s) * R(x^s)^s ) ).

\\ For each j, the coefficient factor is a power series in x^s alone, so the j-th summand has exponents congruent to m*(s-1-j) mod s. Hence only the term with
\\ m*(s-1-j) == m*k (mod s)
\\ can contribute to [x^(s*n+m*k)] G(x). Since gcd(m,s)=1, this gives j = s-1-k. Therefore
\\ [x^(s*n+m*k)] G(x)
\\ =
\\ [x^(s*n)]
\\ 1 / ( P(x^s) * ( Q(x^s)^s - x^(m*s) * R(x^s)^s ) ).

\\ Now let
\\ H(y) = 1 / ( P(y) * ( Q(y)^s - y^m * R(y)^s ) ).
\\ Then H(x^s) = Sum_{n>=0} h(n) * x^(s*n) if H(y) = Sum_{n>=0} h(n) * y^n, so
\\ [x^(s*n)] H(x^s) = [y^n] H(y).
\\ Thus
\\ [x^(s*n+m*k)] G(x)
\\ =
\\ [y^n] 1 / ( P(y) * ( Q(y)^s - y^m * R(y)^s ) ),
\\ equivalently,
\\ Sum_{n>=0} a(s*n+m*k) * y^n
\\ = 1 / ( P(y) * ( Q(y)^s - y^m * R(y)^s ) ).
\\ QED.

check_subsequence_identity(s, m, k, P, Q, R, N=15) = {
  my(t, trunc_deg, Aser, Bser, lhs, rhs, ok=1, vsub, vb);

  if (s < 2, error("Require s >= 2"));
  if (m < 1, error("Require m >= 1"));
  if (k < 0 || k > s-1, error("Require 0 <= k <= s-1"));
  if (gcd(s, m) != 1, error("Require gcd(s,m) = 1"));
  if (polcoef(P, 0) == 0, error("Require P(0) != 0"));
  if (polcoef(Q, 0) == 0, error("Require Q(0) != 0"));
  if (polcoef(R, 0) == 0, error("Require R(0) != 0"));

  t = m * k;
  trunc_deg = s * N + t + 1;

  Aser =
    1 / (subst(P, x, x^s)
       * subst(Q, x, x^s)^(s-1-k)
       * subst(R, x, x^s)^k
       * (subst(Q, x, x^s) - x^m * subst(R, x, x^s))
       + O(x^(trunc_deg + 1)));

  Bser =
    1 / (P * (Q^s - x^m * R^s) + O(x^(N + 1)));

  print("s=", s, ", m=", m, ", k=", k, ", t=", t, ", N=", N);
  print("P(x) = ", P);
  print("Q(x) = ", Q);
  print("R(x) = ", R);
  print("A(x) denominator = ",
    subst(P, x, x^s)
    * subst(Q, x, x^s)^(s-1-k)
    * subst(R, x, x^s)^k
    * (subst(Q, x, x^s) - x^m * subst(R, x, x^s)));
  print("B(x) denominator = ", P * (Q^s - x^m * R^s));

  for (n = 0, N,
    lhs = polcoef(Aser, s*n + t);
    rhs = polcoef(Bser, n);
    if (lhs != rhs,
      ok = 0;
      print("mismatch at n=", n, ": a(", s, "*", n, "+", t, ")=", lhs, ", b(", n, ")=", rhs);
    );
  );

  if (ok,
    print("OK for n = 0..", N);
    vsub = vector(N + 1, j, polcoef(Aser, s*(j-1) + t));
    vb = vector(N + 1, j, polcoef(Bser, j-1));
    print("a(", s, "*n+", t, ") for n = 0..", N, ": ", vsub);
    print("b(n)     for n = 0..", N, ": ", vb);
  );

  ok
};

\\ Examples.
\\ Example 1:
\\ a(2*n+3) from 1/(1 - x^3 - x^4 - x^5) equals coefficients of 1/(1 - x - x^2 - x^4).
check_subsequence_identity(2, 3, 1, 1 + x, 1 - x, 1);
print("A060945");
print("------------------");

\\ Example 2:
\\ a(2*n+5) from 1/(1 - x^4 - x^5 - x^7) equals coefficients of 1/(1 - x - x^2 + x^3 - x^5 - x^6).
check_subsequence_identity(2, 5, 1, 1 + x, 1 - x, 1);

\\ Example 3:
\\ a(3*n+8) from 1/(1 + x^3 - x^4) equals coefficients of 1/(1 + 3*x + 3*x^2 + x^3 - x^4).
check_subsequence_identity(3, 4, 2, 1, 1 + x, 1);
print("A107068");
print("------------------");

\\ Example 4:
\\ a(3*n+4) from 1/(1 - x^2 - x^5 - x^6) equals coefficients of 1/(1 - 2*x - x^2 + x^3 - x^4).
check_subsequence_identity(3, 2, 2, 1 + x, 1 - x, 1);

\\ Example 5:
\\ a(2*n+3) from 1/(1 - x^3 - x^4 - 2*x^5 - x^7) equals coefficients of 1/(1 - 2*x + x^2 - x^3 - 2*x^4 - x^5).
check_subsequence_identity(2, 3, 1, 1, 1 - x, 1 + x);
