\\ a(0) = 1; a(n) = Sum_{k=0..floor((n-1)/2)} binomial(n+1,2*k+2) * a(k).
a_vector(n) = my(v=vector(n+1)); v[1]=1; for(i=1, n, v[i+1]=sum(j=0, (i-1)\2, binomial(i+1, 2*j+2)*v[j+1])); v;
a_vector(31)