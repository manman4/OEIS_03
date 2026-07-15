a_vector(n) = my(v=vector(n)); v[1]=1; for(m=2,n,v[m]=sum(i=1,m-2,sum(j=1,m-i-1,v[i]*v[j]*v[m-i-j]))+sum(r=1,m\2,(-1)^(r+1)*binomial(m-r,r)*v[m-r])); v;
a_vector(30)
