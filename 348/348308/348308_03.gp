M=30;

a(n) = if(n<6, 1, 3*a(n-1)-3*a(n-2)+a(n-3)-a(n-6));
for(n=0, M, print1(a(n), ", "));