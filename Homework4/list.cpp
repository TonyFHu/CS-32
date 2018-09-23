void listAll(const Class* c, string path)
{
	cout << path + c->name() << endl;
	for (int i = 0; i < c->subclasses().size(); i++)
		listAll((c->subclasses())[i], path + c->name() + "=>");
}