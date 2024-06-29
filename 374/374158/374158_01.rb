require 'prime'

def A(n)
  m = Math.sqrt(n).to_i + 1
  ary = Array.new(n + 1, 0)
  (1..m).each{|i|
    (1..m).each{|j|
      k = i * i + 3 * j * j
      if k <= n
        ary[k] += 1
      end
    }
  }
  ary
end

n = 23 * 10 ** 7
ary = A(n)

p a = (0..100).map{|i| ary.index(i)}
# p (0..17).map{|i| (a[i] / 4r).denominator}
(1..30).each{|i|
  j = a[i]
  if j != nil
    p [i, j, [j.prime_division.to_a]]
  end
}
