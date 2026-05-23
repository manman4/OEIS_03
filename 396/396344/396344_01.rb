# a(0) = 0; for n > 0, a(n) = a(n-1) - n if nonnegative and not already in the sequence, otherwise a(n) = a(n-1) + n - 1.
def generate(limit)
  seq = [0]
  seen = {0 => true}

  (1..limit).each do |n|
    candidate = seq[n - 1] - n
    if candidate >= 0 && !seen[candidate]
      val = candidate
    else
      val = seq[n - 1] + n - 1
    end
    seq << val
    seen[val] = true
  end

  seq
end

limit = 100000000
seq = generate(limit)

output_seen = {}
first_n = {}
(0..limit - 2).each do |n|
  if seq[n] == seq[n + 2]
    val = seq[n]
    unless output_seen[val]
      output_seen[val] = true
      first_n[val] = n
      puts val
    end
  end
end
p output_seen.keys
p first_n.values

