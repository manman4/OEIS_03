# a102365(n, k) = if(k==0, 1, if(n<k, 0, (n-k)*a102365(n-1, k-1)+(2*k+1)*a102365(n-1, k)));
# 0..n+1
def A102365(n)
  ary = [[1, 0]]
  (1..n).each{|i|
    ary << [1] + (1..i).map{|j| (i - j) * ary[i - 1][j - 1] + (2 * j + 1) * ary[i - 1][j]} + [0]
  }
  ary
end

# T(n, k) = if(k<0 || k>n, 0, (n-k)*T(n-1, k-1)+2*(k+1)*T(n-1, k)+a102365(n, k));
def A(n)
  b = A102365(n)
  ary = [[1, 0]]
  (1..n).each{|i|
    ary << [2 ** (i + 1) - 1] + (1..i).map{|j| (i - j) * ary[i - 1][j - 1] + 2 * (j + 1) * ary[i - 1][j] + b[i][j]} + [0]
  }
  (0..n).map{|i| (0..i).map{|j| ary[i][j]}}
end

n = 20
ary = A(n)
p ary.flatten
p (0..n).map{|i| ary[i].sum}