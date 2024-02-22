def b(a, n)
  a = [0] + a
  s2 = 0
  (1..n - 1).each{|i|
    s2 += 1 if a[i] == i + 1 && a[i + 1] == i
  }
  return false if s2 > 0
  s3 = 0
  (1..n - 2).each{|i|
    s3 += 1 if a[i] == i + 1 && a[i + 1] == i + 2 && a[i + 2] == i
  }
  return false if s3 > 0
  true
end

def a(n)
  return 1 if n == 0
  cnt = 0
  (1..n).to_a.each_permutation(n){|s|
    if b(s, n)
      cnt += 1
    end
  }
  cnt
end

p (0..12).map{|i| a(i)}
