b(n) = sumdiv(n, d, ((d%7==0)+(d%7==2)+(d%7==5))*d);
a(n) = if(n==0, 1, -(1/n) * sum(k=1, n, b(k) * a(n-k)));
for(n=0, 20, print1(a(n), ", "))

a_vector(n) = my(v=vector(n+1)); v[1]=1; for(i=1, n, v[i+1]= -(1/i) * sum(j=1, i, b(j)*v[i-j+1])); v;
a_vector(98)