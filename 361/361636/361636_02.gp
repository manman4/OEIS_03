default(parisize, 1200000000)

my(v='v, w='w, x='x, y='y, z='z);

R_3 = 1/(1 - (x*y*z + y*z + x*z + x*y));
R_4 = 1/(1 - (w*x*y*z + x*y*z + w*y*z + w*x*z + w*x*y));
R_5 = 1/(1 - (v*w*x*y*z + w*x*y*z + v*x*y*z + v*w*y*z + v*w*x*z + v*w*x*y));

diag(n, expr, var) = {
  my(a = vector(n));
  for (i = 1, #var, expr = taylor(expr, var[#var - i + 1], n));
  for (k = 1, n, a[k] = expr;
    for (i = 1, #var, a[k] = polcoeff(a[k], k-1)));
  return(a);
};

diag(10, R_3,       [x, y, z])
diag(10, R_4,    [w, x, y, z])
diag( 5, R_5, [v, w, x, y, z])