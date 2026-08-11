## How to approach any LLD Problem Step-By-Step:

0. Clarify all the requirements

1. Idenitfy the nouns (Nouns = Things)
    - These become our candidate classes

2. Now identify the verbs and actions
    - These become our methods/behaviours

3. Now Decide Who Owns Each Responsibility
    - Always ask : Can this responsibility move somewhere else? - Don't just dump all responsibilities to one

4. Now Identify Relationships
    - Association : A relationship between two independent objects, both can exist without each other
    - Aggregation : A has B, but B can live without A
    - Composition : A has B, but B can't live without A
    - Prefer Composition (has-a) over Inheritance (is-a)

5. Now idenitfy What Is Likely to Change
    - Here comes the role of Strategy / polymorphism

6. Identify interfaces / abstractions
    - Whenever multiple implementations can exist for the same responsibility, introduce an abstraction

7. Now write down the complete main flow using the nouns and verbs



