require "big"

def ncr(n, r)
  return 1 if r == 0
  ((n - r + 1)..n).reduce(BigInt.new(1)){|p, i| p * i} // (1..r).reduce(BigInt.new(1)){|p, i| p * i}
end

def a(n)
  p a = (0..n - 1).map{|i| ncr(n - 1, i)}
  ary = [0] * (n + 1)
  if n == 3 || n == 4 || n == 6 || n == 8 || n == 12 || n == 14
    i0 = 1
    b = (1..n).to_a.reject{|x| x == i0}
    b.each_permutation{|i|
      i = [i0] + i
      # iとaの交代和を計算
      if (0..n - 1).reduce(BigInt.new(0)){|s, j| s + (-1) ** (j % 2) * a[j] * i[j]} == 0
        ary[i0] += 1
      end
    }
    ((n + 1) // 2 + 1..n).each{|i| ary[i] = ary[n + 1 - i]}
    return ary[1..-1]
  else
    (1..(n + 1) // 2).each{|i0|
      b = (1..n).to_a.reject{|x| x == i0}
      b.each_permutation{|i|
        i = [i0] + i
        # iとaの交代和を計算
        if (0..n - 1).reduce(BigInt.new(0)){|s, j| s + (-1) ** (j % 2) * a[j] * i[j]} == 0
          ary[i0] += 1
        end
      }
    }
    ((n + 1) // 2 + 1..n).each{|i| ary[i] = ary[n + 1 - i]}
    return ary[1..-1]
  end
end

# n = 13で４時間くらいかかる
n = 14
p ary = (n..n).map{|i| a(i)}.flatten
(1..ary.size).each{|i|
  print i
  print ' '
  puts ary[i - 1]
}
