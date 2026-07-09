sumcubes(n)=sqr((n*(n+1))\2);

B8(N)=
{
  my(M2=N\2, M4=N\4, s=1, k=1, q1,q2,q4, u1,u2,u4,u, a,b,c);
  while(k<=N,
    q1=N\k; q2=M2\k; q4=M4\k;
    u1=N\q1;
    u2=if(q2==0, N, M2\q2);
    u4=if(q4==0, N, M4\q4);
    u=min(u1, min(u2, u4));
    a=(u*(u+1))\2;
    b=((k-1)*k)\2;
    c=16*q1 - 32*q2 + 256*q4;
    s += c*(a*a - b*b);
    k=u+1
  );
  s
};

a(n)=B8(10^n);

for(n=0, 16, write("b373896_1.txt", n, " ", a(n)));


