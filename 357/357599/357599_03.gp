M=21;

a(n) = if(n<3, 0^n-(-1)^n, (-1)^(n+1)*(n+1)!/4);
for(n=0, M, print1(a(n), ", "));