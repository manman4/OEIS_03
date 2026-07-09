\\ Cf. A373896.

sumcubes(n)=n^2*(n+1)^2/4;
S3(N)=
{
  my(s=0, k=1, t, u);
  while(k<=N,
    t=N\k;
    u=N\t;
    s += t*(sumcubes(u)-sumcubes(k-1));
    k=u+1
  );
  s
};

B8(N) = 1 + 16*S3(N) - 32*S3(N\2) + 256*S3(N\4);

for(n=0, 10000, write("b341397.txt", n, " ", B8(n)));


