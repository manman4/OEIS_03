# a(n) = 1 + Sum_{i, j, k>=0 and i+j+k=n-1} (i+1) a(i) * a(j) * a(k).
def A(n)
  ary = [1]
  (1..n).each{|i|
    sum = 0
    (0..i - 1).each{|j|
      (0..i - 1 - j).each{|k|
        sum += (j + 1) * ary[j] * ary[k] * ary[i - 1 - j - k]
      }
    }
    ary << 1 + sum
  }
  ary
end

n = 450
ary = A(n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}