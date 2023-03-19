default(parisize, 1200000000)

my(x='x, y='y, z='z, w='w, v='v);

R_3 = 1/(1 - (x*y*z + y*z + x*z + x*y));
R_4 = 1/(1 - (w*x*y*z + x*y*z + w*y*z + w*x*z + w*x*y));
R_5 = 1/(1 - v*w*x*y*z * (1 + 1/v + 1/w + 1/x + 1/y + 1/z));

diag(n, expr, var) = {
  my(a = vector(n));
  for (i = 1, #var, expr = taylor(expr, var[#var - i + 1], n));
  for (k = 1, n, a[k] = expr;
    for (i = 1, #var, a[k] = polcoeff(a[k], k-1)));
  return(a);
};

diag(10, R_3, [x, y, z])
diag(10, R_4, [x, y, z, w])
diag(11, R_5, [x, y, z, w, v]) \\ [1, 1, 1, 1, 121, 721, 2521, 6721, 128521, 1277641, 7539841]