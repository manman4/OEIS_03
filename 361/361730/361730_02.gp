default(parisize, 1200000000)

my(x='x, y='y, z='z, w='w, v='v);

diag(n, expr, var) = {
  my(a = vector(n));
  for (i = 1, #var, expr = taylor(expr, var[#var - i + 1], n));
  for (k = 1, n, a[k] = expr;
    for (i = 1, #var, a[k] = polcoeff(a[k], k-1)));
  return(a);
};

R = 1/(1 - (1 + x*y*z) * (x^3 + y^3 + z^3)); 
diag(16, R, [x, y, z]) 
