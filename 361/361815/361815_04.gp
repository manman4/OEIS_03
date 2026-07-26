default(parisize, 1200000000)

my(x='x, y='y, z='z, w='w, v='v);

diag(n, expr, var) = {
  my(a = vector(n));
  for (i = 1, #var, expr = taylor(expr, var[#var - i + 1], n));
  for (k = 1, n, a[k] = expr;
    for (i = 1, #var, a[k] = polcoeff(a[k], k-1)));
  return(a);
};

diagonal_rational(s, t, e, vars) = {
  if (#vars != s || #e != s,
    error("The vectors vars and e must have length s")
  );
  if (sum(i = 1, s, e[i]) != t,
    error("The sum of the exponents must be t")
  );
  my(X = prod(i = 1, s, vars[i]));
  1 / (1 - sum(i = 1, s,
    vars[i] * (1 - X)^e[i]
  ))
};

R = 1/(1 - (1 - x*y) * (x + y)); 
diag(35, R, [x, y]) 

\\ For integers t >= 0 and 0 <= e <= t, [x^n*y^n] 1/(1 - x*(1-x*y)^e - y*(1-x*y)^(t-e)) = Sum_{k=0..n} (-1)^(n-k) * binomial(2*k,k) * binomial(t*k,n-k).
R = diagonal_rational(2, 2, [2, 0], [x, y]);
diag(16, R, [x, y])

R = diagonal_rational(2, 2, [1, 1], [x, y]);
diag(16, R, [x, y])

