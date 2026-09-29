import A374275Lean.GoldenExistence

/-!
# A374275: unconditional divisibility theorem

The public theorem is `A374275Lean.eleven_dvd_a374275`.

The proof combines the unconditional factor-replacement theorem in the golden
integer ring, existence of every positive representation count, and the exact
finite definition and minimality of the OEIS sequence.
-/

namespace A374275Lean

/-- Every positive-index term of A374275 is divisible by `11`. -/
theorem eleven_dvd_a374275 {n : ℕ} (hn : 0 < n) :
    11 ∣ a374275 n :=
  eleven_dvd_a374275_of_replacement GoldenRing.elevenReplacement hn
    (GoldenRing.exists_representationCount_pos n hn)

end A374275Lean
