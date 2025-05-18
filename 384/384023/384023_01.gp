b(n) = {
  if(n==0, return(1));
  my(sum = 0);
    for(i=0, 2*n,
      for(j=0, 2*n,
          for(k=0, 2*n,
            if(i + j + k == 2*n,
              sum += abs(stirling(i+n-1, n-1, 2) * stirling(j+n-1, n-1, 2) * stirling(k+n-1, n-1, 2));
            )
          )
      )
    );
  sum;
};

for(n=0, 20, print1(b(n),", "));

a(n) = if(n==0, 1, sum(i=0, 2*n, sum(j=0, 2*n-i, stirling(i+n-1, n-1, 2)*stirling(j+n-1, n-1, 2)*stirling(3*n-1-i-j, n-1, 2))));
for(n=0, 20, print1(a(n)-b(n),", "));
