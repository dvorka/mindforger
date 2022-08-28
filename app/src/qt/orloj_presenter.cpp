/*
 outline_view.h     MindForger thinking notebook

 Copyright (C) 2016-2022 Martin Dvorak <martin.dvorak@mindforger.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#include "orloj_presenter.h"

#include "../../lib/src/gear/lang_utils.h"

using namespace std;

namespace m8r {

OrlojPresenter::OrlojPresenter(MainWindowPresenter* mainPresenter,
                               OrlojView* view,
                               Mind* mind)
    : activeFacet{OrlojPresenterFacets::FACET_NONE},
      config{Configuration::getInstance()},
      skipEditNoteCheck{false}
{
    this->mainPresenter = mainPresenter;
    this->view = view;
    this->mind = mind;

    this->dashboardPresenter = new DashboardPresenter(view->getDashboard(), this);
    this->organizersTablePresenter = new OrganizersTablePresenter(view->getOrganizersTable(), mainPresenter->getHtmlRepresentation());
    this->organizerPresenter = new OrganizerPresenter(view->getOrganizer(), this);
    this->kanbanPresenter = new KanbanPresenter(view->getKanban(), this);
    this->tagCloudPresenter = new TagsTablePresenter(view->getTagCloud(), mainPresenter->getHtmlRepresentation());
    this->outlinesTablePresenter = new OutlinesTablePresenter(view->getOutlinesTable(), mainPresenter->getHtmlRepresentation());
    this->recentNotesTablePresenter = new RecentNotesTablePresenter(view->getRecentNotesTable(), mainPresenter->getHtmlRepresentation());
    this->outlineViewPresenter = new OutlineViewPresenter(view->getOutlineView(), this);
    this->outlineHeaderViewPresenter = new OutlineHeaderViewPresenter(view->getOutlineHeaderView(), this);
    this->outlineHeaderEditPresenter = new OutlineHeaderEditPresenter(view->getOutlineHeaderEdit(), mainPresenter, this);
    this->noteViewPresenter = new NoteViewPresenter(view->getNoteView(), this);
    this->noteEditPresenter = new NoteEditPresenter(view->getNoteEdit(), mainPresenter, this);
    this->navigatorPresenter = new NavigatorPresenter(view->getNavigator(), this, mind->getKnowledgeGraph());

    /* Orloj presenter WIRES signals and slots between VIEWS and PRESENTERS.
     *
     * It's done by Orloj presenter as it has access to all its child windows
     * and widgets - it can show/hide what's needed and then pass control to children.
     */

    // hit enter in organizers to view organizer detail
    QObject::connect(
        view->getOrganizersTable(),
        SIGNAL(signalShowSelectedOrganizer()),
        this,
        SLOT(slotShowSelectedOrganizer()));
    // hit enter in Os to view O detail
    QObject::connect(
        view->getOutlinesTable(),
        SIGNAL(signalShowSelectedOutline()),
        this,
        SLOT(slotShowSelectedOutline()));
    QObject::connect(
        view->getDashboard()->getOutlinesDashboardlet(),
        SIGNAL(signalShowSelectedOutline()),
        this,
        SLOT(slotShowSelectedOutline()));
    QObject::connect(
        view->getOutlinesTable(),
        SIGNAL(signalFindOutlineByName()),
        mainPresenter,
        SLOT(doActionFindOutlineByName()));
    // click O tree to view Note
    QObject::connect(
        view->getOutlineView()->getOutlineTree()->selectionModel(),
        SIGNAL(selectionChanged(QItemSelection, QItemSelection)),
        this,
        SLOT(slotShowNote(QItemSelection, QItemSelection)));
    // hit ENTER in recent Os/Ns to view O/N detail
    QObject::connect(
        view->getRecentNotesTable(),
        SIGNAL(signalShowSelectedRecentNote()),
        this,
        SLOT(slotShowSelectedRecentNote()));
    QObject::connect(
        view->getDashboard()->getRecentDashboardlet(),
        SIGNAL(signalShowSelectedRecentNote()),
        this,
        SLOT(slotShowSelectedRecentNote()));
    // hit ENTER in Tags to view Recall by Tag detail
    QObject::connect(
        view->getTagCloud(),
        SIGNAL(signalShowDialogForTag()),
        this,
        SLOT(slotShowSelectedTagRecallDialog()));
    QObject::connect(
        view->getDashboard()->getTagsDashboardlet(),
        SIGNAL(signalShowDialogForTag()),
        this,
        SLOT(slotShowSelectedTagRecallDialog()));
    // navigator
    QObject::connect(
        navigatorPresenter, SIGNAL(signalOutlineSelected(Outline*)),
        this, SLOT(slotShowOutlineNavigator(Outline*)));
    QObject::connect(
        navigatorPresenter, SIGNAL(signalNoteSelected(Note*)),
        this, SLOT(slotShowNoteNavigator(Note*)));
    QObject::connect(
        navigatorPresenter, SIGNAL(signalThingSelected()),
        this, SLOT(slotShowNavigator()));
    // N editor
#ifdef __APPLE__
    QObject::connect(
        mainPresenter->getMainMenu()->getView()->actionEditComplete, SIGNAL(triggered()),
        this, SLOT(slotEditStartLinkCompletion()));
#endif
    // editor getting data from the backend
    QObject::connect(
        view->getNoteEdit()->getNoteEditor(), SIGNAL(signalGetLinksForPattern(QString)),
        this, SLOT(slotGetLinksForPattern(QString)));
    QObject::connect(
        this, SIGNAL(signalLinksForPattern(QString, std::vector<std::string>*)),
        view->getNoteEdit()->getNoteEditor(), SLOT(slotPerformLinkCompletion(QString, std::vector<std::string>*)));
    QObject::connect(
        view->getOutlineHeaderEdit()->getHeaderEditor(), SIGNAL(signalGetLinksForPattern(QString)),
        this, SLOT(slotGetLinksForPattern(QString)));
    QObject::connect(
        this, SIGNAL(signalLinksForHeaderPattern(QString, std::vector<std::string>*)),
        view->getOutlineHeaderEdit()->getHeaderEditor(), SLOT(slotPerformLinkCompletion(QString, std::vector<std::string>*)));
    QObject::connect(
        outlineHeaderEditPresenter->getView()->getButtonsPanel(), SIGNAL(signalShowLivePreview()),
        mainPresenter, SLOT(doActionToggleLiveNotePreview()));
    QObject::connect(
        noteEditPresenter->getView()->getButtonsPanel(), SIGNAL(signalShowLivePreview()),
        mainPresenter, SLOT(doActionToggleLiveNotePreview()));
    // intercept Os table column sorting
    QObject::connect(
        view->getOutlinesTable()->horizontalHeader(), SIGNAL(sectionClicked(int)),
        this, SLOT(slotOutlinesTableSorted(int)));
    // toggle full O HTML preview
    QObject::connect(
        view->getOutlineHeaderView()->getEditPanel()->getFullOPreviewButton(), SIGNAL(clicked()),
        this, SLOT(slotToggleFullOutlinePreview()));
    // show O header @ N
    QObject::connect(
        view->getNoteView()->getButtonsPanel()->getShowOutlineHeaderButton(), SIGNAL(clicked()),
        this, SLOT(slotShowOutlineHeader()));
}

int dialogSaveOrCancel(QWidget* view)
{
    // l10n by moving this dialog either to Qt class OR view class
    QMessageBox msgBox{
        QMessageBox::Question,
        QObject::tr("Save Note"),
        QObject::tr("Do you want to save changes?"),
        {},
        view
    };

    QPushButton* discard = msgBox.addButton(
#ifdef __APPLE__
        QObject::tr("Discard changes"),
#else
        QObject::tr("&Discard changes"),
#endif
        QMessageBox::DestructiveRole
    );
    QPushButton* autosave = msgBox.addButton(
#ifdef __APPLE__
        QObject::tr("Autosave"),
#else
        QObject::tr("Autosave"),
#endif
        QMessageBox::AcceptRole
    );
    autosave->setToolTip(QObject::tr("Do not ask & autosave"));
    QPushButton* edit = msgBox.addButton(
#ifdef __APPLE__
        QObject::tr("Continue editing"),
#else
        QObject::tr("Continue &editing"),
#endif
        QMessageBox::YesRole
    );
    QPushButton* save = msgBox.addButton(
#ifdef __APPLE__
        QObject::tr("Save"),
#else
        QObject::tr("&Save"),
#endif
        QMessageBox::ActionRole
    );
    msgBox.exec();

    QAbstractButton* choosen = msgBox.clickedButton();
    if(discard == choosen) {
        return OrlojButtonRoles::DISCARD_ROLE;
    } else if(autosave == choosen) {
        return OrlojButtonRoles::AUTOSAVE_ROLE;
    } else if(edit == choosen) {
        return OrlojButtonRoles::EDIT_ROLE;
    } else if(save == choosen) {
        return OrlojButtonRoles::SAVE_ROLE;
    }

    return OrlojButtonRoles::INVALID_ROLE;
}

void OrlojPresenter::onFacetChange(const OrlojPresenterFacets targetFacet) const
{
    MF_DEBUG("Facet CHANGE: " << activeFacet << " > " << targetFacet << endl);

    if(activeFacet == OrlojPresenterFacets::FACET_NAVIGATOR) {
        navigatorPresenter->cleanupBeforeHide();
    } else if(targetFacet == OrlojPresenterFacets::FACET_VIEW_OUTLINE) {
        outlineViewPresenter->getOutlineTree()->focus();
    }
}

void OrlojPresenter::showFacetRecentNotes(const vector<Note*>& notes)
{
    setFacet(OrlojPresenterFacets::FACET_RECENT_NOTES);
    recentNotesTablePresenter->refresh(notes);
    view->showFacetRecentNotes();
    mainPresenter->getStatusBar()->showMindStatistics();
}

void OrlojPresenter::showFacetDashboard() {
    setFacet(OrlojPresenterFacets::FACET_DASHBOARD);

    vector<Note*> allNotes{};
    mind->getAllNotes(allNotes, true, true);
    map<const Tag*,int> allTags{};
    mind->getTagsCardinality(allTags);

    dashboardPresenter->refresh(
        mind->getOutlines(),
        allNotes,
        allTags,
        mind->remind().getOutlineMarkdownsSize(),
        mind->getStatistics()
    );
    view->showFacetDashboard();
    mainPresenter->getMainMenu()->showFacetDashboard();
    mainPresenter->getStatusBar()->showMindStatistics();
}

void OrlojPresenter::showFacetOrganizerList(const vector<Organizer*>& organizers)
{
    setFacet(OrlojPresenterFacets::FACET_LIST_ORGANIZERS);
    // IMPROVE reload ONLY if dirty, otherwise just show
    organizersTablePresenter->refresh(organizers);
    view->showFacetOrganizers();
    mainPresenter->getMainMenu()->showFacetOrganizerList();
    mainPresenter->getStatusBar()->showMindStatistics();
}

void OrlojPresenter::showFacetEisenhowerMatrix(
    Organizer* organizer,
    const vector<Note*>& outlinesAndNotes,
    const vector<Outline*>& outlines,
    const vector<Note*>& notes
) {
    setFacet(OrlojPresenterFacets::FACET_ORGANIZER);
    organizerPresenter->refresh(
        organizer,
        outlinesAndNotes,
        outlines,
        notes
    );
    view->showFacetOrganizer();
    mainPresenter->getMainMenu()->showFacetOrganizer();
    mainPresenter->getStatusBar()->showInfo(tr("Eisenhower Matrix: ")+QString::fromStdString(
        organizer->getName())
    );
}

void OrlojPresenter::showFacetKanban(
    Kanban* kanban,
    const vector<Note*>& outlinesAndNotes,
    const vector<Outline*>& outlines,
    const vector<Note*>& notes
) {
    setFacet(OrlojPresenterFacets::FACET_KANBAN);
    kanbanPresenter->refresh(
        kanban,
        outlinesAndNotes,
        outlines,
        notes
    );
    view->showFacetKanban();
    // Kanban shares menu facet with Eisenhower Matrix as both are organizers
    mainPresenter->getMainMenu()->showFacetOrganizer();
    mainPresenter->getStatusBar()->showInfo(
        tr("Kanban: ")+QString::fromStdString(kanban->getName())
    );
}

void OrlojPresenter::showFacetKnowledgeGraphNavigator()
{
    switch(activeFacet) {
    case OrlojPresenterFacets::FACET_VIEW_OUTLINE_HEADER:
    case OrlojPresenterFacets::FACET_EDIT_OUTLINE_HEADER:
    case OrlojPresenterFacets::FACET_VIEW_OUTLINE:
        setFacet(OrlojPresenterFacets::FACET_NAVIGATOR);
        navigatorPresenter->showInitialView(outlineViewPresenter->getCurrentOutline());
        slotShowOutlineNavigator(outlineViewPresenter->getCurrentOutline());
        break;
    case OrlojPresenterFacets::FACET_VIEW_NOTE:
        setFacet(OrlojPresenterFacets::FACET_NAVIGATOR);
        navigatorPresenter->showInitialView(noteViewPresenter->getCurrentNote());
        slotShowNoteNavigator(noteViewPresenter->getCurrentNote());
        break;
    case OrlojPresenterFacets::FACET_EDIT_NOTE:
        setFacet(OrlojPresenterFacets::FACET_NAVIGATOR);
        navigatorPresenter->showInitialView(noteEditPresenter->getCurrentNote());
        slotShowNoteNavigator(noteEditPresenter->getCurrentNote());
        break;
    case OrlojPresenterFacets::FACET_TAG_CLOUD:
        setFacet(OrlojPresenterFacets::FACET_NAVIGATOR);
        navigatorPresenter->showInitialViewTags();
        view->showFacetNavigator();
        break;
    default:
        setFacet(OrlojPresenterFacets::FACET_NAVIGATOR);
        navigatorPresenter->showInitialView();
        view->showFacetNavigator();
        break;
    }

    mainPresenter->getMainMenu()->showFacetNavigator();
    mainPresenter->getStatusBar()->showMindStatistics();
}

void OrlojPresenter::showFacetOutlineList(const vector<Outline*>& outlines)
{
    setFacet(OrlojPresenterFacets::FACET_LIST_OUTLINES);
    // IMPROVE reload ONLY if dirty, otherwise just show
    outlinesTablePresenter->refresh(outlines);
    view->showFacetOutlines();
    mainPresenter->getMainMenu()->showFacetOutlineList();
    mainPresenter->getStatusBar()->showMindStatistics();
}

void OrlojPresenter::slotShowOutlines()
{
    showFacetOutlineList(mind->getOutlines());
}

void OrlojPresenter::slotShowSelectedOrganizer()
{
    static vector<Note*> organizerOutlinesAndNotes{};
    static vector<Note*> organizerNotes{};

    if(activeFacet!=OrlojPresenterFacets::FACET_VIEW_OUTLINE
         &&
       activeFacet!=OrlojPresenterFacets::FACET_TAG_CLOUD
         &&
       activeFacet!=OrlojPresenterFacets::FACET_NAVIGATOR
         &&
       activeFacet!=OrlojPresenterFacets::FACET_RECENT_NOTES
      )
    {
        int row = organizersTablePresenter->getCurrentRow();
        if(row != OrganizersTablePresenter::NO_ROW) {
            QStandardItem* item{organizersTablePresenter->getModel()->item(row)};
            // TODO introduce name my user role - replace constant with my enum name > do it for whole file e.g. MfDataRole
            if(item) {
                Organizer* organizer = item->data(Qt::UserRole + 1).value<Organizer*>();
                MF_DEBUG("Organizer selected by Orloj: data(user)=" << organizer << endl);

                // ensure O scope validity
                if(organizer->getOutlineScope().size()) {
                    if(!mind->remind().getOutline(organizer->getOutlineScope())) {
                        organizer->clearOutlineScope();
                    }
                }

                organizerOutlinesAndNotes.clear();
                organizerNotes.clear();
                if(Organizer::OrganizerType::KANBAN == organizer->getOrganizerType()) {
                    showFacetKanban(
                        dynamic_cast<Kanban*>(organizer),
                        mind->getAllNotes(organizerOutlinesAndNotes, true, true),
                        mind->getOutlines(),
                        mind->getAllNotes(organizerNotes, true, false)
                    );
                } else {
                    // Eisnehower Matrix as fallback
                    showFacetEisenhowerMatrix(
                        dynamic_cast<EisenhowerMatrix*>(organizer),
                        mind->getAllNotes(organizerOutlinesAndNotes, true, true),
                        mind->getOutlines(),
                        mind->getAllNotes(organizerNotes, true, false)
                    );
                }
                string statusNotebookScope{
                    organizer->getOutlineScope().size()
                    ?" scope: '"+mind->remind().getOutline(organizer->getOutlineScope())->getName()+"'"
                    :""
                };
                mainPresenter->getStatusBar()->showInfo(
                    QString("%1%2%3%4")
                        .arg(tr("Organizer: '"))
                        .arg(organizer->getName().c_str())
                        .arg("'")
                        .arg(statusNotebookScope.c_str())
                );

                mainPresenter->sortAndSaveOrganizersConfig();

                return;
            } else {
                mainPresenter->getStatusBar()->showInfo(QString(tr("Selected Organizer not found!")));
            }
        }
        mainPresenter->getStatusBar()->showInfo(QString(tr("No Organizer selected!")));
    }
}

void OrlojPresenter::showFacetOutline(Outline* outline)
{
    if(activeFacet == OrlojPresenterFacets::FACET_NAVIGATOR) {
        outlineHeaderViewPresenter->refresh(outline);
        view->showFacetNavigatorOutline();
    } else {
        setFacet(OrlojPresenterFacets::FACET_VIEW_OUTLINE);

        outlineViewPresenter->refresh(outline);
        outlineHeaderViewPresenter->refresh(outline);
        view->showFacetOutlineHeaderView();
    }

    outline->incReads();
    outline->makeDirty();

    mainPresenter->getMainMenu()->showFacetOutlineView();

    mainPresenter->getStatusBar()->showInfo(QString("Notebook '%1'   %2").arg(outline->getName().c_str()).arg(outline->getKey().c_str()));
}

void OrlojPresenter::slotShowSelectedOutline()
{
    if(activeFacet!=OrlojPresenterFacets::FACET_ORGANIZER
         &&
       activeFacet!=OrlojPresenterFacets::FACET_TAG_CLOUD
         &&
       activeFacet!=OrlojPresenterFacets::FACET_NAVIGATOR
         &&
       activeFacet!=OrlojPresenterFacets::FACET_RECENT_NOTES
      )
    {
        int row;
        if(activeFacet==OrlojPresenterFacets::FACET_DASHBOARD) {
            row = dashboardPresenter->getOutlinesPresenter()->getCurrentRow();
        } else {
            row = outlinesTablePresenter->getCurrentRow();
        }
        if(row != OutlinesTablePresenter::NO_ROW) {
            QStandardItem* item;
            if(activeFacet==OrlojPresenterFacets::FACET_DASHBOARD) {
                item = dashboardPresenter->getOutlinesPresenter()->getModel()->item(row);
            } else {
                item = outlinesTablePresenter->getModel()->item(row);
            }
            // TODO introduce name my user role - replace constant with my enum name > do it for whole file e.g. MfDataRole
            if(item) {
                Outline* outline = item->data(Qt::UserRole + 1).value<Outline*>();
                showFacetOutline(outline);
                return;
            } else {
                mainPresenter->getStatusBar()->showInfo(QString(tr("Selected Notebook not found!")));
            }
        }
        mainPresenter->getStatusBar()->showInfo(QString(tr("No Notebook selected!")));
    }
}

void OrlojPresenter::slotShowOutline(const QItemSelection& selected, const QItemSelection& deselected)
{
    Q_UNUSED(deselected);

    if(activeFacet!=OrlojPresenterFacets::FACET_ORGANIZER
         &&
       activeFacet!=OrlojPresenterFacets::FACET_TAG_CLOUD
         &&
       activeFacet!=OrlojPresenterFacets::FACET_NAVIGATOR
         &&
       activeFacet!=OrlojPresenterFacets::FACET_RECENT_NOTES
      )
    {
        QModelIndexList indices = selected.indexes();
        if(indices.size()) {
            const QModelIndex& index = indices.at(0);
            QStandardItem* item = outlinesTablePresenter->getModel()->itemFromIndex(index);
            // TODO introduce name my user role - replace constant with my enum name > do it for whole file e.g. MfDataRole
            Outline* outline = item->data(Qt::UserRole + 1).value<Outline*>();
            showFacetOutline(outline);
        } else {
            mainPresenter->getStatusBar()->showInfo(QString(tr("No Notebook selected!")));
        }
    }
}

void OrlojPresenter::showFacetTagCloud()
{
    setFacet(OrlojPresenterFacets::FACET_TAG_CLOUD);

    map<const Tag*,int> tags{};
    mind->getTagsCardinality(tags);
    tagCloudPresenter->refresh(tags);

    view->showFacetTagCloud();

    mainPresenter->getStatusBar()->showInfo(QString("%2 Tags").arg(tags.size()));
}

void OrlojPresenter::slotShowSelectedTagRecallDialog()
{
    if(activeFacet == OrlojPresenterFacets::FACET_TAG_CLOUD
         ||
       activeFacet == OrlojPresenterFacets::FACET_DASHBOARD
      )
    {
        int row;
        if(activeFacet==OrlojPresenterFacets::FACET_DASHBOARD) {
            row = dashboardPresenter->getTagsPresenter()->getCurrentRow();
        } else {
            row = tagCloudPresenter->getCurrentRow();
        }
        if(row != OutlinesTablePresenter::NO_ROW) {
            QStandardItem* item;
            switch(activeFacet) {
            case OrlojPresenterFacets::FACET_TAG_CLOUD:
                item = tagCloudPresenter->getModel()->item(row);
                break;
            case OrlojPresenterFacets::FACET_DASHBOARD:
                item = dashboardPresenter->getTagsPresenter()->getModel()->item(row);
                break;
            default:
                item = nullptr;
            }
            // TODO introduce name my user role - replace constant with my enum name > do it for whole file e.g. MfDataRole
            if(item) {
                const Tag* tag = item->data(Qt::UserRole + 1).value<const Tag*>();
                mainPresenter->doTriggerFindNoteByTag(tag);
            } else {
                mainPresenter->getStatusBar()->showInfo(QString(tr("Selected Tag not found!")));
            }
        } else {
            mainPresenter->getStatusBar()->showInfo(QString(tr("No Tag selected!")));
        }
    }
}

void OrlojPresenter::slotShowTagRecallDialog(const QItemSelection& selected, const QItemSelection& deselected)
{
    Q_UNUSED(deselected);

    if(activeFacet == OrlojPresenterFacets::FACET_TAG_CLOUD
         ||
       activeFacet == OrlojPresenterFacets::FACET_DASHBOARD
      )
    {
        QModelIndexList indices = selected.indexes();
        if(indices.size()) {
            const QModelIndex& index = indices.at(0);
            QStandardItem* item;
            // TODO if 2 switch
            if(activeFacet == OrlojPresenterFacets::FACET_TAG_CLOUD) {
                item = tagCloudPresenter->getModel()->itemFromIndex(index);
            } else {
                item = dashboardPresenter->getTagsPresenter()->getModel()->itemFromIndex(index);
            }
            // TODO introduce name my user role - replace constant with my enum name > do it for whole file e.g. MfDataRole
            const Tag* tag = item->data(Qt::UserRole + 1).value<const Tag*>();
            mainPresenter->doTriggerFindNoteByTag(tag);
        } else {
            mainPresenter->getStatusBar()->showInfo(QString(tr("No Tag selected!")));
        }
    }
}

void OrlojPresenter::showFacetNoteView()
{
    view->showFacetNoteView();
    mainPresenter->getMainMenu()->showFacetOutlineView();
    setFacet(OrlojPresenterFacets::FACET_VIEW_NOTE);
}

void OrlojPresenter::showFacetNoteView(Note* note)
{
    if(activeFacet == OrlojPresenterFacets::FACET_NAVIGATOR) {
        noteViewPresenter->refresh(note);
        view->showFacetNavigatorNote();
        mainPresenter->getMainMenu()->showFacetOutlineView();
    } else {
        if(outlineViewPresenter->getCurrentOutline()!=note->getOutline()) {
            showFacetOutline(note->getOutline());
        }
        noteViewPresenter->refresh(note);
        view->showFacetNoteView();
        outlineViewPresenter->selectRowByNote(note);
        mainPresenter->getMainMenu()->showFacetOutlineView();

        setFacet(OrlojPresenterFacets::FACET_VIEW_NOTE);
    }

    QString p{note->getOutline()->getKey().c_str()};
    p += "#";
    p += note->getMangledName().c_str();
    mainPresenter->getStatusBar()->showInfo(QString(tr("Note '%1'   %2")).arg(note->getName().c_str()).arg(p));
}

void OrlojPresenter::showFacetNoteEdit(Note* note)
{
    if(activeFacet == OrlojPresenterFacets::FACET_EDIT_NOTE) {
        MF_DEBUG("Facet will NOT be changed to FACET_EDIT_NOTE as N is being already edited" << endl);
        return;
    }

    if(activeFacet == OrlojPresenterFacets::FACET_NAVIGATOR) {
        outlineViewPresenter->refresh(note->getOutline());
    }

    if(mainPresenter->withWriteableOutline(note->getOutline()->getKey())) {
        noteEditPresenter->setNote(note);
        view->showFacetNoteEdit();
        setFacet(OrlojPresenterFacets::FACET_EDIT_NOTE);
        mainPresenter->getMainMenu()->showFacetNoteEdit();

        // refresh live preview to ensure on/off autolinking, full O vs. header, ...
        if(config.isUiLiveNotePreview()) {
            noteViewPresenter->refreshLivePreview();
        }

        // ensure that editor gets focus (might be stole by live preview)
        view->getNoteEdit()->giveEditorFocus();
    }
}

void OrlojPresenter::showFacetOutlineHeaderEdit(Outline* outline)
{
    if(activeFacet == OrlojPresenterFacets::FACET_EDIT_OUTLINE_HEADER) {
        MF_DEBUG(
            "Facet will NOT be changed to FACET_EDIT_OUTLINE_HEADER as "
            "header is being already edited" << endl
        );
        return;
    }

    if(activeFacet == OrlojPresenterFacets::FACET_NAVIGATOR) {
        outlineViewPresenter->refresh(outline);
    }

    if(mainPresenter->withWriteableOutline(outline->getKey())) {
        outlineHeaderEditPresenter->setOutline(outline);
        view->showFacetOutlineHeaderEdit();
        setFacet(OrlojPresenterFacets::FACET_EDIT_OUTLINE_HEADER);
        mainPresenter->getMainMenu()->showFacetNoteEdit();

        // refresh live preview to ensure on/off autolinking, full O vs. header, ...
        if(config.isUiLiveNotePreview()) {
            outlineHeaderViewPresenter->refreshLivePreview();
        }
    }
}

bool OrlojPresenter::applyFacetHoisting()
{
    if(Configuration::getInstance().isUiHoistedMode()) {
        if(isFacetActive(OrlojPresenterFacets::FACET_VIEW_OUTLINE)
             ||
           isFacetActive(OrlojPresenterFacets::FACET_VIEW_OUTLINE_HEADER))
        {
            view->showFacetHoistedOutlineHeaderView();
        } else if(isFacetActive(OrlojPresenterFacets::FACET_VIEW_NOTE)) {
            view->showFacetHoistedNoteView();
        } else if(isFacetActive(OrlojPresenterFacets::FACET_EDIT_NOTE)) {
            view->showFacetHoistedNoteEdit();
        } else if(isFacetActive(OrlojPresenterFacets::FACET_EDIT_OUTLINE_HEADER)) {
            view->showFacetHoistedOutlineHeaderEdit();
        }

        return false;
    } else {
        if(isFacetActive(OrlojPresenterFacets::FACET_VIEW_OUTLINE)
             ||
           isFacetActive(OrlojPresenterFacets::FACET_VIEW_OUTLINE_HEADER))
        {
            view->showFacetOutlineHeaderView();
        } else if(isFacetActive(OrlojPresenterFacets::FACET_VIEW_NOTE)) {
            view->showFacetNoteView();
        } else if(isFacetActive(OrlojPresenterFacets::FACET_EDIT_NOTE)) {
            view->showFacetNoteEdit();
        } else if(isFacetActive(OrlojPresenterFacets::FACET_EDIT_OUTLINE_HEADER)) {
            view->showFacetOutlineHeaderEdit();
        }

        return true;
    }
}

void OrlojPresenter::fromOutlineHeaderEditBackToView(Outline* outline)
{
    // LEFT: update O name above Ns tree
    outlineViewPresenter->refresh(outline->getName());
    // RIGHT
    outlineHeaderViewPresenter->refresh(outline);
    view->showFacetOutlineHeaderView();
    setFacet(OrlojPresenterFacets::FACET_VIEW_OUTLINE_HEADER);
    mainPresenter->getMainMenu()->showFacetOutlineView();
}

void OrlojPresenter::fromNoteEditBackToView(Note* note)
{
    noteViewPresenter->clearSearchExpression();
    noteViewPresenter->refresh(note);
    // update N in the tree
    outlineViewPresenter->refresh(note);

    showFacetNoteView();
}

bool OrlojPresenter::avoidDataLossOnLinkClick()
{
    return this->avoidDataLossOnNoteEdit();
}

bool OrlojPresenter::avoidDataLossOnNoteEdit()
{
    // avoid lost of N editor changes
    if(skipEditNoteCheck) {
        skipEditNoteCheck=false;
        if(activeFacet == OrlojPresenterFacets::FACET_EDIT_NOTE) {
            noteEditPresenter->getView()->getNoteEditor()->setFocus();
        } else if(activeFacet == OrlojPresenterFacets::FACET_EDIT_OUTLINE_HEADER) {
            outlineHeaderEditPresenter->getView()->getHeaderEditor()->setFocus();
        }
        return true;
    } else {
        if(activeFacet == OrlojPresenterFacets::FACET_EDIT_NOTE) {
            int decision{};
            if(Configuration::getInstance().isUiEditorAutosave()) {
                decision = OrlojButtonRoles::SAVE_ROLE;
            } else {
                decision = dialogSaveOrCancel(view);
            }

            switch(decision) {
            case OrlojButtonRoles::DISCARD_ROLE:
                // do nothing
                break;
            case OrlojButtonRoles::AUTOSAVE_ROLE:
                Configuration::getInstance().setUiEditorAutosave(true);
                mainPresenter->getConfigRepresentation()->save(Configuration::getInstance());
                MF_FALL_THROUGH;
            case OrlojButtonRoles::SAVE_ROLE:
                noteEditPresenter->slotSaveNote();
                break;
            case OrlojButtonRoles::EDIT_ROLE:
                MF_FALL_THROUGH;
            default:
                // rollback ~ select previous N and continue
                // ugly & stupid hack to disable signal emitted on N selection in O tree
                skipEditNoteCheck=true;
                outlineViewPresenter->selectRowByNote(noteEditPresenter->getCurrentNote());
                return true;
            }
        } else if(activeFacet == OrlojPresenterFacets::FACET_EDIT_OUTLINE_HEADER) {
            int decision = dialogSaveOrCancel(view);
            switch(decision) {
            case OrlojButtonRoles::DISCARD_ROLE:
                // do nothing
                break;
            case OrlojButtonRoles::AUTOSAVE_ROLE:
                Configuration::getInstance().setUiEditorAutosave(true);
                mainPresenter->getConfigRepresentation()->save(Configuration::getInstance());
                MF_FALL_THROUGH;
            case OrlojButtonRoles::SAVE_ROLE:
                outlineHeaderEditPresenter->slotSaveOutlineHeader();
                break;
            case OrlojButtonRoles::EDIT_ROLE:
                MF_FALL_THROUGH;
            default:
                // rollback ~ select previous N and continue
                // ugly & stupid hack to disable signal emitted on N selection in O tree
                skipEditNoteCheck=true;
                outlineViewPresenter->getOutlineTree()->clearSelection();
                return true;
            }
        }
    }

    return false;
}

void OrlojPresenter::slotShowOutlineHeader()
{
    if(avoidDataLossOnNoteEdit()) return;

    // refresh header
    outlineHeaderViewPresenter->refresh(outlineViewPresenter->getCurrentOutline());
    setFacet(OrlojPresenterFacets::FACET_VIEW_OUTLINE_HEADER);
    view->showFacetOutlineHeaderView();
}

void OrlojPresenter::slotShowNote(const QItemSelection& selected, const QItemSelection& deselected)
{
    Q_UNUSED(deselected);

    if(avoidDataLossOnNoteEdit()) return;

    QModelIndexList indices = selected.indexes();
    if(indices.size()) {
        const QModelIndex& index = indices.at(0);
        QStandardItem* item
            = outlineViewPresenter->getOutlineTree()->getModel()->itemFromIndex(index);
        // IMPROVE make my role constant
        Note* note = item->data(Qt::UserRole + 1).value<Note*>();

        note->incReads();
        note->makeDirty();

        showFacetNoteView(note);
    } else {
        Outline* outline = outlineViewPresenter->getCurrentOutline();
        mainPresenter->getStatusBar()->showInfo(QString("Notebook '%1'   %2").arg(outline->getName().c_str()).arg(outline->getKey().c_str()));
    }
}

void OrlojPresenter::slotShowNavigator()
{
    view->showFacetNavigator();
}

void OrlojPresenter::slotShowNoteNavigator(Note* note)
{
    if(note) {
        note->incReads();
        note->makeDirty();

        showFacetNoteView(note);
    }
}

void OrlojPresenter::slotShowOutlineNavigator(Outline* outline)
{
    if(outline) {
        // timestamps are updated by O header view
        showFacetOutline(outline);
    }
}

/**
 * @brief Return MD links for given O/N name prefix (pattern).
 *
 * For example 'Mi' > { '[MindForger](mf/projects.md#mind-forger)', '[Middle](mf/places.md#middle) }'
 */
void OrlojPresenter::slotGetLinksForPattern(const QString& pattern)
{
    vector<Thing*> allThings{};
    vector<string> thingsNames = vector<string>{};
    string prefix{pattern.toStdString()};

    Outline* currentOutline;
    if(activeFacet == OrlojPresenterFacets::FACET_EDIT_OUTLINE_HEADER) {
        currentOutline = outlineHeaderEditPresenter->getCurrentOutline();
    } else {
        currentOutline = noteEditPresenter->getCurrentNote()->getOutline();
    }

    mind->getAllThings(
        allThings,
        &thingsNames,
        &prefix,
        ThingNameSerialization::LINK,
        currentOutline);

    vector<string>* links = new vector<string>{};
    *links = thingsNames;

    if(activeFacet == OrlojPresenterFacets::FACET_EDIT_OUTLINE_HEADER) {
        emit signalLinksForHeaderPattern(pattern, links);
    } else {
        emit signalLinksForPattern(pattern, links);
    }
}

void OrlojPresenter::slotShowSelectedRecentNote()
{
    if(activeFacet == OrlojPresenterFacets::FACET_RECENT_NOTES
         ||
       activeFacet == OrlojPresenterFacets::FACET_DASHBOARD
      )
    {
        int row;
        if(activeFacet==OrlojPresenterFacets::FACET_DASHBOARD) {
            row = dashboardPresenter->getRecentNotesPresenter()->getCurrentRow();
        } else {
            row = recentNotesTablePresenter->getCurrentRow();
        }
        if(row != RecentNotesTablePresenter::NO_ROW) {
            QStandardItem* item;
            switch(activeFacet) {
            case OrlojPresenterFacets::FACET_RECENT_NOTES:
                item = recentNotesTablePresenter->getModel()->item(row);
                break;
            case OrlojPresenterFacets::FACET_DASHBOARD:
                item = dashboardPresenter->getRecentNotesPresenter()->getModel()->item(row);
                break;
            default:
                item = nullptr;
            }
            // TODO make my role constant
            if(item) {
                const Note* note = item->data(Qt::UserRole + 1).value<const Note*>();

                showFacetOutline(note->getOutline());
                if(note->getType() != note->getOutline()->getOutlineDescriptorNoteType()) {
                    // IMPROVE make this more efficient
                    showFacetNoteView();
                    getOutlineView()->selectRowByNote(note);
                }
                mainPresenter->getStatusBar()->showInfo(QString(tr("Note "))+QString::fromStdString(note->getName()));
            } else {
                mainPresenter->getStatusBar()->showInfo(QString(tr("Selected Notebook/Note not found!")));
            }
        } else {
            mainPresenter->getStatusBar()->showInfo(QString(tr("No Note selected!")));
        }
    }
}

void OrlojPresenter::slotShowRecentNote(const QItemSelection& selected, const QItemSelection& deselected)
{
    Q_UNUSED(deselected);

    if(activeFacet == OrlojPresenterFacets::FACET_RECENT_NOTES
         ||
       activeFacet == OrlojPresenterFacets::FACET_DASHBOARD
      )
    {
        QModelIndexList indices = selected.indexes();
        if(indices.size()) {
            const QModelIndex& index = indices.at(0);
            QStandardItem* item;
            if(activeFacet == OrlojPresenterFacets::FACET_RECENT_NOTES) {
                item = recentNotesTablePresenter->getModel()->itemFromIndex(index);
            } else {
                item = dashboardPresenter->getRecentNotesPresenter()->getModel()->itemFromIndex(index);
            }
            // TODO make my role constant
            const Note* note = item->data(Qt::UserRole + 1).value<const Note*>();

            showFacetOutline(note->getOutline());
            if(note->getType() != note->getOutline()->getOutlineDescriptorNoteType()) {
                // IMPROVE make this more efficient
                showFacetNoteView();
                getOutlineView()->selectRowByNote(note);
            }
            mainPresenter->getStatusBar()->showInfo(QString(tr("Note "))+QString::fromStdString(note->getName()));
        } else {
            mainPresenter->getStatusBar()->showInfo(QString(tr("No Note selected!")));
        }
    }
}

void OrlojPresenter::refreshLiveNotePreview()
{
    if(isFacetActive(OrlojPresenterFacets::FACET_EDIT_NOTE)) {
        view->showFacetNoteEdit();
    } else if(isFacetActive(OrlojPresenterFacets::FACET_EDIT_OUTLINE_HEADER)) {
        view->showFacetOutlineHeaderEdit();
    }
}

void OrlojPresenter::slotEditStartLinkCompletion()
{
    if(isFacetActive(OrlojPresenterFacets::FACET_EDIT_NOTE)) {
        view->getNoteEdit()->getNoteEditor()->slotStartLinkCompletion();
    } else if(isFacetActive(OrlojPresenterFacets::FACET_EDIT_OUTLINE_HEADER)) {
        view->getOutlineHeaderEdit()->getHeaderEditor()->slotStartLinkCompletion();
    }
}

void OrlojPresenter::slotRefreshCurrentNotePreview()
{
    MF_DEBUG("Slot to refresh live preview: " << getFacet() << " hoist: " << config.isUiHoistedMode() << endl);
    if(!config.isUiHoistedMode()) {
        if(isFacetActive(OrlojPresenterFacets::FACET_EDIT_NOTE)) {
            noteViewPresenter->refreshLivePreview();
#if defined(__APPLE__) || defined(_WIN32)
            getNoteEdit()->getView()->getNoteEditor()->setFocus();
#endif
        } else if(isFacetActive(OrlojPresenterFacets::FACET_EDIT_OUTLINE_HEADER)) {
            outlineHeaderViewPresenter->refreshLivePreview();
#if defined(__APPLE__) || defined(_WIN32)
            getOutlineHeaderEdit()->getView()->getHeaderEditor()->setFocus();
#endif
        }
    }
}

void OrlojPresenter::slotOutlinesTableSorted(int column)
{
    Qt::SortOrder order
        = view->getOutlinesTable()->horizontalHeader()->sortIndicatorOrder();
    MF_DEBUG("Os table sorted: " << column << " descending: " << order << endl);

    config.setUiOsTableSortColumn(column);
    config.setUiOsTableSortOrder(order==Qt::SortOrder::AscendingOrder?true:false);
    mainPresenter->getConfigRepresentation()->save(config);
}

void OrlojPresenter::slotToggleFullOutlinePreview()
{
    config.setUiFullOPreview(!config.isUiFullOPreview());
    mainPresenter->getConfigRepresentation()->save(config);

    // refresh O header view
    getOutlineHeaderView()->refreshCurrent();
}

} // m8r namespace
