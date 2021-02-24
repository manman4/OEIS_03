def A(n)
  cnt = 0
  (1..n - 1).each{|i|
    (1..n - 1).each{|j|
      if i * i + j * j == (n - i) * (n - i)
        cnt += 1
      end
    }
  }
  cnt
end

p (1..200).select{|i| A(i) > 0}
p (1..20).map{|i| A(i * i)}

def B(n)
  cnt = 0
  (1..n - 1).each{|i|
    j = i * i + (n - i) * (n - i)
    if Math.sqrt(j).to_i ** 2 == j
      cnt += 1
    end
  }
  cnt
end
p (1..100).map{|i| B(i)}
