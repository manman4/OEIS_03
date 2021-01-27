@ary = (0..9).to_a + ('A'..'Z').to_a

def A(n)
  d = 0
  a = (1..n - 1).map{|i| [i]}
  s = n - 1
  while d < n - 2
    d += 1
    b = []
    a.each{|i|
      (1..n - 1).each{|j|
        m = i.clone + [j]
        b << m if (0..d).inject(0){|s, k| s + m[k] * n ** (d - k)} % (d + 1) == 0
      }
    }
    a = b
    s += a.size
    p ["(" + a[-1].map{|i| @ary[i]}.join + ")_#{n}", (0..d).inject(0){|s, k| s + a[-1][k] * n ** (d - k)}] if d == n - 2
  end
  p s
  s
end

p (2..15).map{|i| A(i)}
