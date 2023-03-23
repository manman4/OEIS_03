my(x='x, y='y);

diag(n, expr, var) = {
  my(a = vector(n));
  for (i = 1, #var, expr = taylor(expr, var[#var - i + 1], n));
  for (k = 1, n, a[k] = expr;
    for (i = 1, #var, a[k] = polcoeff(a[k], k-1)));
  return(a);
};

R = 1/(1 - (1 + (x*y)^3) * (x + y));
diag(30, R, [x, y]) 