\\ For integers s>=1, t>=0 and e_1,...,e_s>=0 with
\\ e_1+...+e_s=t,

\\ [x_1^n*...*x_s^n]
\\ 1/(1 - Sum_{i=1..s} x_i*(1+x_1*...*x_s)^e_i)
\\ =
\\ Sum_{k=0..n} (s*k)!/(k!)^s * binomial(t*k,n-k).

\\ The choice s=t=3 and e_1=e_2=e_3=1 gives
\\ 1/(1-(1+x*y*z)*(x+y+z)), the rational function appearing
\\ in the name of this sequence.

\\ Proof: Writing X=x_1*...*x_s and
\\ A=Sum_{i=1..s} x_i*(1+X)^e_i,
\\ the expansion 1/(1-A)=Sum_{m>=0} A^m is used. A term of
\\ A^m obtained by selecting the i-th summand r_i times is

\\ m!/(r_1!*...*r_s!) *
\\ x_1^r_1*...*x_s^r_s *
\\ (1+X)^(e_1*r_1+...+e_s*r_s),

\\ where r_1+...+r_s=m.

\\ A power X^j from the last factor changes the exponents of
\\ all the variables by the same amount j. Its contribution to
\\ [x_1^n*...*x_s^n] therefore requires

\\ r_1+j=...=r_s+j=n.

\\ Thus r_1=...=r_s=k and m=s*k. The corresponding part of
\\ A^(s*k) is

\\ (s*k)!/(k!)^s * X^k *
\\ (1+X)^(k*(e_1+...+e_s))
\\ =
\\ (s*k)!/(k!)^s * X^k*(1+X)^(t*k).

\\ Its coefficient of X^n is

\\ (s*k)!/(k!)^s * [X^(n-k)](1+X)^(t*k)
\\ =
\\ (s*k)!/(k!)^s * binomial(t*k,n-k).

\\Summing over k=0,...,n proves the identity.


default(parisize, 1200000000)

my(x='x, y='y, z='z, w='w, v='v);

diag(n, expr, var) = {
  my(a = vector(n));
  for (i = 1, #var, expr = taylor(expr, var[#var - i + 1], n));
  for (k = 1, n, a[k] = expr;
    for (i = 1, #var, a[k] = polcoeff(a[k], k-1)));
  return(a);
};

s0 = 3;
t0 = 3;
e0 = [1, 1, 1];
vars0 = [x, y, z];
X0 = prod(i=1, s0, vars0[i]);

if (sum(i=1, s0, e0[i]) != t0, error("The sum of the exponents must be t"));
R = 1/(1 - sum(i=1, s0, vars0[i]*(1+X0)^e0[i]));

diag(16, R, vars0)
