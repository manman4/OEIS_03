M=30;

a(n) = if(n<8, 1, 4*a(n-1)-6*a(n-2)+4*a(n-3)-a(n-4)-a(n-8));
for(n=0, M, print1(a(n), ", "));