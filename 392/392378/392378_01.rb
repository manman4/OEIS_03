# A(n,0) = 0^n.
# A(0,1) = A(1,1) = 1;   A(n,1) =         Sum_{j=1..n-1} A(j,1) * A(n-j,j).
# For k > 1, A(0,k) = 1; A(n,k) = (1/n) * Sum_{j=1..n} ((k+1)*j-n) * A(j,1) * A(n-j,k).
# 配列で保存して再利用する。
def A(n)
  ary = Array.new(n + 1){Array.new(n + 1)}
  (0..n).each{|i|
    (0..n).each{|j|
      if j == 0
        ary[i][j] = i == 0 ? 1 : 0
      elsif j == 1
        if i == 0 || i == 1
          ary[i][j] = 1
        else
          ary[i][j] = (1..i - 1).inject(0){|s, k| s + ary[k][j] * ary[i - k][k]} # A(n-j,j) の j は k であることに注意。
        end
      else
        if i == 0
          ary[i][j] = 1
        else
          ary[i][j] = (1..i).inject(0){|s, k| s + ((j + 1) * k - i) * ary[k][1] * ary[i - k][j]} / i
        end
      end
    }
  }
  ary
end

# 対角成分A(k,n-k)をとる
def B(n)
  ary = A(n)
  (0..n).map{|i| (0..i).map{|j| ary[j][i - j]}}.flatten
end

n = 139
ary = B(n)
(0..ary.size - 1).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}

