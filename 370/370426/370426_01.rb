def b(a, n, k)
  a = [0] + a
  s = 0
  (1..n - 1).each{|i|
    s += 1 if a[i] == i + 1 && a[i + 1] == i
  }
  return false if s != k
  true
end

def a(n, k)
  return 0 if n < 2
  cnt = 0
  (1..n).to_a.permutation(n){|s|
    if b(s, n, k)
      cnt += 1
    end
  }
  cnt
end

p (0..11).map{|i| a(i, 2)}
