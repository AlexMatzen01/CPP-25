teamproject - Business Trip Expense Calculator

A small C++ console program (chapter 6 exercise) that collects travel expenses for a business trip and calculates:
- Total expenses (what the employee paid)
- Total allowable expenses (company-allowed amounts)
- Amount to be reimbursed (if expenses exceed allowed)

1. Follow prompts in order. Example input sequence (one per prompt):
   - Number of days: 5
   - Departure time (HH.MM): 05.30
   - Return time (HH.MM): 06.45
   - Airfare: 590
   - Car rental: 350
   - Miles driven with private vehicle: 0
   - Parking total: 34
   - Taxi total: 27
   - Registration fees: 125
   - Nightly hotel rate: 55
   - Meals for each day (the program asks only for allowed meals per first/last/middle days)

Outputs
- Total expenses
- Allowable expenses
- Amount to be reimbursed (or $0.00)

Flowchart

```mermaid
graph TD
  Start([Start]) --> AskDays["Ask: total days"]
  AskDays --> AskDeparture["Ask: departure time (HH.MM)"]
  AskDeparture --> AskReturn["Ask: return time (HH.MM)"]
  AskReturn --> AskFixed["Ask: airfare, car rental, private miles"]
  AskFixed --> AskOther["Ask: parking total, taxi total, registration fees, nightly hotel rate"]
  AskOther --> PerDay["For each day (1..N) - collect meals"]

  subgraph Per-Day Meal Rules
    direction TB
    Day1["Day 1: ask meals allowed by departure time"]
    Middle["Middle days: ask B/L/D for each day"]
    Last["Last day: ask meals allowed by arrival time"]
  end

  PerDay --> SumMeals["Sum actual meal totals \n(employee-paid) and allowed/capped meal totals"]
  SumMeals --> ComputeHotel["Compute hotel = nightly * (days - 1)"]
  ComputeHotel --> ComputeTotals["Compute totals:\n- totalExpenses = airfare + carRental + privateVehicle + parking + taxi + registration + hotel + actualMeals\n- allowable = airfare + carRental + privateVehicle + allowedParking + allowedTaxi + registration + allowedHotel + allowedMeals"]
  ComputeTotals --> Print["Print totals and reimbursement"]
  Print --> End([End])

  %% Connect per-day subnodes to PerDay for clarity
  PerDay --> Day1
  PerDay --> Middle
  PerDay --> Last
```

Notes
- Meal caps: breakfast $9, lunch $12, dinner $16
- Parking allowed up to $6/day; taxi allowed up to $10/day; hotel allowed up to $90/night
- Input validation rejects negative values and invalid times

If you'd like, I can:
- Add a non-interactive mode (read inputs from a file)
- Add an example test script that pipes inputs to the executable
- Add a short section describing how each allowance is computed
