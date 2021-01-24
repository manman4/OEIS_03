my(x='x, y='y, z='z, w='w);
R = 1 / (1 - (x + y + z + w) + w*x*y*z);
diag(n, expr, var) = {
  my(a = vector(n));
  for (i = 1, #var, expr = taylor(expr, var[#var - i + 1], n));
  for (k = 1, n, a[k] = expr;
       for (i = 1, #var, a[k] = polcoeff(a[k], k-1)));
  return(a);
};
diag(10, R, [x, y, z, w])

N=10;
diag(n, expr, var=variables(expr)) = {
  my(a=vector(n));
  for(i=1, #var, expr=taylor(expr, var[#var-i+1], n));
  for(j=1, n, a[j]=expr;
    for(i=1, #var, a[j]=polcoeff(a[j], j-1)));
  return(a);
};
R_1(k) = 1/(1+y+z+x*y+y*z+k*z*x+(k+1)*x*y*z);
R_2(k) = 1/((1-x)*(1-y)*(1-z)-k*x*y*z);
for(k=-10, 10, v_1=diag(N, R_1(k)); v_2=diag(N, R_2(k)); if(v_1==v_2, print(v_1)))








my(x='x, y='y, z='z, w='w, v='v);
R = 1 / (1 - (x + y + z + w + v) + v*w*x*y*z);
diag(n, expr, var) = {
  my(a = vector(n));
  for (i = 1, #var, expr = taylor(expr, var[#var - i + 1], n));
  for (k = 1, n, a[k] = expr;
       for (i = 1, #var, a[k] = polcoeff(a[k], k-1)));
  return(a);
};
diag(10, R, [x, y, z, w, v])

my(x='x);

R = 1 / (1 - (x) + x);

diag(n, expr, var) = {

  my(a = vector(n));

  for (i = 1, #var, expr = taylor(expr, var[#var - i + 1], n));

  for (k = 1, n, a[k] = expr;

       for (i = 1, #var, a[k] = polcoeff(a[k], k-1)));

  return(a);

};

diag(10, R, [x])



my(x='x, y='y, z='z, w='w, v='v);
R = 1/(1 + (x*y + x*z + x*w + x*v + y*z + y*w + y*v + z*w + z*v + w*v));
diag(n, expr, var) = {
  my(a = vector(n));
  for (i = 1, #var, expr = taylor(expr, var[#var - i + 1], n));
  for (k = 1, n, a[k] = expr;
       for (i = 1, #var, a[k] = polcoeff(a[k], k-1)));
  return(a);
};
diag(20, R, [x, y, z, w, v])



